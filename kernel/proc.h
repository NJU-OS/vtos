#ifndef VTOS_KERNEL_PROC_H
#define VTOS_KERNEL_PROC_H
struct pcb_regs {
    uint64_t sp;
    uint64_t pc;
    uint64_t spsr;
    uint64_t x[31];
} __aligned(16);

struct run_info {
    uint64_t entry;
    uint64_t load_addr;
    struct mobj *mobj_code;
    struct mobj *mobj_stack;
    struct tee_mmu_info *mmu;
};

#define NUM_PROCS 16
#define NUM_PRIO 8

// p_rts_flags
#define P_SENDING (1 << 0) 
#define P_RECVING (1 << 1)
// p_misc_flags
#define P_DELIEVE (1 << 0)
#define P_INTER   (1 << 1)
// p_getfrom
#define PROC_ANY (-1)

struct proc {
    struct pcb_regs regs;
    struct pcb_regs *uregs;
    uint64_t map;
    struct pgt_cache pgt_cache;
    uint32_t p_num;
    int p_endpoint;
    uint32_t time_res;
    uint32_t p_prio;
    uint32_t p_priv;
    uint32_t p_rts_flags;
    uint32_t p_misc_flags;
    uint32_t p_pending;
    struct message p_sendmsg;
    int p_sendto;
    struct message p_recvmsg;
    int p_getfrom;
    void* p_recvaddr;
    struct run_info run_info;
    uint64_t k_stack;
    struct proc* p_caller_q;
    struct proc* p_q_link;
    struct list_head link;
} __aligned(16);

extern struct proc procs[];
extern struct list_head run_queues[];
struct proc *get_proc(void);
void sn_sched(void);
int sn_thread_alloc_and_run(void* ta);
int enqueue(struct proc *p);
int enqueue_head(struct proc *p);
int proc_fork(struct proc *proc);
void proc_init(void);
TEE_Result sn_tee_ta_exec(void* ta_addr, size_t pn);

void sn_putc(uint8_t ch);
void sn_printf(const char* fmt, ...) __printf(1, 2);
void sn_test(void);

#endif // VTOS_KERNEL_PROC_H
