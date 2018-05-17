#include <platform_config.h>

#include <kernel/user_ta.h>
#include <kernel/spinlock.h>
#include <mm/core_mmu.h>
#include <string.h>
#include <proc.h>


#ifdef CFG_WITH_ARM_TRUSTED_FW
#define STACK_TMP_OFFS      0
#else
#define STACK_TMP_OFFS      SM_STACK_TMP_RESERVE_SIZE
#endif

#ifdef ARM64
#define STACK_TMP_SIZE      (2048 + STACK_TMP_OFFS)
#define STACK_THREAD_SIZE   8192
#endif /*ARM64*/

#ifdef CFG_WITH_STACK_CANARIES
#ifdef ARM64
#define STACK_CANARY_SIZE   (8 * sizeof(uint32_t))
#endif
#define START_CANARY_VALUE  0xdededede
#define END_CANARY_VALUE    0xabababab
#define GET_START_CANARY(name, stack_num) name[stack_num][0]
#define GET_END_CANARY(name, stack_num) \
    name[stack_num][sizeof(name[stack_num]) / sizeof(uint32_t) - 1]
#else
#define STACK_CANARY_SIZE   0
#endif

#define DECLARE_STACK(name, num_stacks, stack_size, linkage) \
linkage uint32_t name[num_stacks] \
        [ROUNDUP(stack_size + STACK_CANARY_SIZE, STACK_ALIGNMENT) / \
        sizeof(uint32_t)] \
        __attribute__((section(".nozi_stack"), \
                   aligned(STACK_ALIGNMENT)))

#define STACK_SIZE(stack) (sizeof(stack) - STACK_CANARY_SIZE / 2)

#define GET_STACK(stack) \
    ((vaddr_t)(stack) + STACK_SIZE(stack))

extern struct cpu_local cpu_locals[];
struct proc procs[NUM_PROCS];
struct list_head run_queues[NUM_PRIO];

DECLARE_STACK(stack_proc, NUM_PROCS, STACK_THREAD_SIZE, static);

static unsigned int proc_global_lock = SPINLOCK_UNLOCK;

static void lock_global(void)
{
    cpu_spin_lock(&proc_global_lock);
}

static void unlock_global(void)
{
    cpu_spin_unlock(&proc_global_lock);
}

struct proc *get_proc(void)
{
    struct cpu_local *cloc = get_cpu_local();
    int cur = cloc->cur_proc;
    if(cur>=0 && cur<NUM_PROCS && procs[cur].p_endpoint==cur)
        return &procs[cur];
    return NULL;
}

int enqueue(struct proc* p) {
    struct list_head *lh;
    struct list_head *lp;
    if(p==NULL || p->p_rts_flags!=0)
        return -1;
    lh = &run_queues[p->p_prio];
    lp = &p->link;
    lp->prev = lh->prev;
    lp->prev->next = lp;
    lp->next = lh;
    lh->prev = lp;
    return 0;
}

int enqueue_head(struct proc* p) {
    struct list_head *lh;
    struct list_head *lp;
    if(p==NULL || p->p_rts_flags!=0)
        return -1;
    lh = &run_queues[p->p_prio];
    lp = &p->link;
    lp->next = lh->next;
    lp->next->prev = lp;
    lp->prev = lh;
    lh->next = lp;
    return 0;
}

void sn_thread_std_smc_entry(void);
int call_resume(struct pcb_regs *regs, uint32_t spsr);
int sn_thread_alloc_and_run(void* ta)
{
    size_t n;
    //struct thread_core_local *l = thread_get_core_local();
    //struct cpu_local *l = get_cpu_local();
    bool found_proc = false;
    struct proc* proc;
    uint32_t spsr = SPSR_64(SPSR_64_MODE_EL1, SPSR_64_MODE_SP_EL0, 0);
    spsr |= read_daif();

    //assert(l->cur_proc == -1);

    lock_global();

    for (n = 0; n < NUM_PROCS; n++) {
        if (procs[n].p_endpoint == -1) {
            procs[n].p_endpoint = n;
            found_proc = true;
            break;
        }
    }

    unlock_global();

    if (!found_proc) {
        DMSG("\nSNOW proc alloc error!\n");
        return -1;
    }

    //l->cur_proc = n;

    proc = &procs[n];
    proc->regs.pc = (uint64_t)sn_thread_std_smc_entry;

    /*
     * Stdcalls starts in SVC mode with masked foreign interrupts, masked
     * Asynchronous abort and unmasked native interrupts.
     */
    proc->regs.spsr = SPSR_64(SPSR_64_MODE_EL1, SPSR_64_MODE_SP_EL0,
                THREAD_EXCP_FOREIGN_INTR | DAIFBIT_ABT);
    /* Reinitialize stack pointer */
    proc->regs.sp = proc->k_stack;
    proc->p_prio = 4;

    /*
     * Copy arguments into context. This will make the
     * arguments appear in x0-x7 when thread is started.
     */
    proc->regs.x[0] = (uint64_t)ta;
    proc->regs.x[1] = (uint64_t)n;
    /* Set up frame pointer as per the Aarch64 AAPCS */
    proc->regs.x[29] = 0;

    //thread_lazy_save_ns_vfp();
    return call_resume(&procs[n].regs, spsr);
}

int proc_fork(struct proc *proc)
{
    size_t n;
    bool found_proc = false;
    struct proc* p_ch;
    TEE_Result res;
    int mp_id = proc->uregs->x[0];

    if(mp_id<0 || procs[mp_id].p_endpoint!=mp_id)
        return -1;
    proc = &procs[mp_id];
    lock_global();

    for (n = 0; n < NUM_PROCS; n++) {
        if (procs[n].p_endpoint == -1) {
            procs[n].p_endpoint = n;
            found_proc = true;
            break;
        }
    }

    unlock_global();

    if (!found_proc) {
        DMSG("\nSNOW proc alloc error!\n");
        return -1;
    }

    p_ch = &procs[n];

    p_ch->p_prio = proc->p_prio;
    *(p_ch->uregs) = *(proc->uregs);
    p_ch->p_rts_flags = proc->p_rts_flags;
    p_ch->p_getfrom = proc->p_getfrom;
    p_ch->p_recvaddr = proc->p_recvaddr;
    res = ta_copy(proc, p_ch);
    if(res != TEE_SUCCESS) {
        DMSG("\nSNOW ta_copy error!\n");
        return -1;
    }
    return n;
}

vaddr_t get_tmp_stack(int i);
void proc_init(void)
{
    int i;
    for(i=0; i<NUM_PROCS; i++) {
        procs[i].k_stack = GET_STACK(stack_proc[i]);
        procs[i].uregs = (void*)(procs[i].k_stack - sizeof(struct pcb_regs));
        procs[i].p_endpoint = -1;
        SLIST_INIT(&procs[i].pgt_cache);
    }
    for(i=0; i<CFG_TEE_CORE_NB_CORE; i++) {
        cpu_locals[i].cur_proc = -1;
        cpu_locals[i].tmp_stack = get_tmp_stack(i);
    }
    for(i=0; i<NUM_PRIO; i++) {
        struct list_head *tp = &run_queues[i];
        run_queues[i].next = tp;
        run_queues[i].prev = tp;
    }
}

void proc_resume(struct pcb_regs *regs);
void sn_sched(void) {
    struct cpu_local *cloc = get_cpu_local();
    struct core_mmu_user_map map = { 0 };
    struct proc *proc = NULL;
    struct list_head* lh;
    int i;
    //lock_global();
    for(i=0; i<NUM_PRIO; i++) {
        lh = &run_queues[i];
        if(lh->next != lh) {
            proc = container_of(lh->next, struct proc, link);
            lh->next = lh->next->next;
            lh->next->prev = lh;
            break;
        }
    }
    //proc = proc_head.next;
    if(proc == NULL)
        DMSG("sn_sched error\n");
    //DMSG("sn_sched proc %d\n", proc->p_endpoint);
    //proc->next->prev = &proc_head;
    //proc_head.next = proc->next;
    //unlock_global();
    assert(proc->p_endpoint>=0);
    cloc->cur_proc = proc->p_endpoint;
    map.user_map = proc->map;
    core_mmu_set_user_map(&map);
    if(proc->p_misc_flags & P_DELIEVE) {
        memcpy(proc->p_recvaddr, (void*)&proc->p_recvmsg, sizeof(struct message));
        proc->p_misc_flags &= (~P_DELIEVE);
        //DMSG("test %s\n", (proc->p_recvmsg).msg);
    }
    //thread_lazy_save_ns_vfp();
    if(proc->p_misc_flags & P_INTER) {
        proc->p_misc_flags &= (~P_INTER);
        proc_resume(&(proc->regs));
    } else{
        proc_resume(proc->uregs);
    }
}

TEE_Result sn_tee_ta_exec(void* ta_addr, size_t pn)
{
    TEE_Result res;
    struct proc *proc = &procs[pn];
    //struct list_head *lh;
    //struct list_head *lp;

    assert(pn>=0);
    //ta = (void*)0x6100000ul;
    //ta = (void*)0x61226c4ul;
    res = sn_ta_load(ta_addr, proc);

    if (res != TEE_SUCCESS) {
        DMSG("init session failed 0x%x", res);
        return res;
    }
    enqueue(proc);
    return res;
}
