#include <util.h>
#include <compiler.h>
#include <mm/pgt_cache.h>
#include <proc.h>
#include <tee/tee_svc.h>
#include <string.h>
#include <message.h>

int syscall_send(struct proc *proc)
{
	int who = (int)(proc->uregs->x[0]);
	void *msg = (void*)(proc->uregs->x[1]);
	struct proc *dproc;
	struct message *dmsg;

	assert(proc != NULL && proc->p_rts_flags==0);
	
	//DMSG("%d:sn send to %d\n", proc->p_endpoint, who);
	if(!(who>=0 && who<NUM_PROCS)) {
		return -1;
	}
	
	dproc = &procs[who];
	if( (dproc->p_rts_flags & P_RECVING) && 
		(dproc->p_getfrom == PROC_ANY || dproc->p_getfrom == proc->p_endpoint)) {
		// msg copy
		//DMSG("%d:sn send 1\n", proc->p_endpoint);
		dmsg = &dproc->p_recvmsg;
		memcpy(dmsg, msg, sizeof(struct message));
		dmsg->from = proc->p_endpoint;
		dmsg->to = who;
		// proc flags change
		dproc->p_rts_flags &= (~P_RECVING);
		dproc->p_misc_flags |= P_DELIEVE;
		dproc->uregs->x[0] = 0;
		if(dproc->p_rts_flags == 0) {
			enqueue(dproc);			
		}
	} else{
		// msg copy
		//DMSG("%d:sn send 2\n", proc->p_endpoint);
		proc->p_rts_flags |= P_SENDING;
		proc->p_sendto = who;
		dmsg = &proc->p_sendmsg; 	
		memcpy(dmsg, msg, sizeof(struct message));
		dmsg->from = proc->p_endpoint;
		dmsg->to = who;
		// dest p_caller_q link
		if(dproc->p_caller_q == NULL)
			dproc->p_caller_q = proc;
		else{
			struct proc *tmp = dproc->p_caller_q;
			while(tmp->p_q_link != NULL)
				tmp = tmp->p_q_link;	
			tmp->p_q_link = proc;
		}
		proc->p_q_link = NULL;
	}
	//DMSG("SNOW I am here\n");
	return 0;
}

int syscall_receive(struct proc *proc)
{
	int who = (int)(proc->uregs->x[0]);
	void *msg = (void*)(proc->uregs->x[1]);
	struct proc *sproc;
	struct message *smsg;
	struct proc* tmp = NULL;

	assert(proc != NULL && proc->p_rts_flags==0);
	
	if(!(who==PROC_ANY || (who>=0 && who<NUM_PROCS))) {
		return -1;
	}
	
	//DMSG("%d:sn recv from %d\n", proc->p_endpoint, who);
	if(proc->p_pending) {
		struct message m;
		m.to = who;
		m.type = M_TYPE_NOTIFY;	
		m.u.ts = 2;
		memcpy(msg, &m, sizeof(struct message));
		proc->p_pending = 0;
		return 0;
	}
	sproc = proc->p_caller_q;
	while(sproc != NULL) {
		if(who==PROC_ANY || who==sproc->p_endpoint)
			break;
		tmp = sproc;
		sproc = sproc->p_q_link;
	}
	if(sproc != NULL) {
		assert((sproc->p_rts_flags & P_SENDING) && sproc->p_sendto==proc->p_endpoint);
		// msg copy
		//DMSG("%d:sn recv 1\n", proc->p_endpoint);
		smsg = &sproc->p_sendmsg;
		memcpy(msg, smsg, sizeof(struct message));
		assert(smsg->from = sproc->p_endpoint);
		// proc flags change
		if(tmp == NULL)
			proc->p_caller_q = sproc->p_q_link;
		else
			tmp->p_q_link = sproc->p_q_link;
		sproc->p_rts_flags &= (~P_SENDING);
		sproc->uregs->x[0] = 0;
		if(sproc->p_rts_flags == 0) {
			enqueue(sproc);
		}
	}else {
		//DMSG("%d:sn recv 2\n", proc->p_endpoint);
		proc->p_rts_flags |= P_RECVING;
		proc->p_getfrom = who;
		proc->p_recvaddr = msg;
	}
	return 0;
}

int syscall_sendrec(struct proc *proc)
{
	int who = (int)(proc->uregs->x[0]);
	void *msg = (void*)(proc->uregs->x[1]);
	struct proc *dproc, *sproc, *tmp;
	struct message *dmsg, *smsg;

	assert(proc != NULL && proc->p_rts_flags==0);
	
	//DMSG("%d:sn send to %d\n", proc->p_endpoint, who);
	tmp = NULL;
	if(!(who>=0 && who<NUM_PROCS)) {
		return -1;
	}
	
	dproc = &procs[who];
	if( (dproc->p_rts_flags & P_RECVING) && 
		(dproc->p_getfrom == PROC_ANY || dproc->p_getfrom == proc->p_endpoint)) {
		// msg copy
		//DMSG("%d:sn send 1\n", proc->p_endpoint);
		dmsg = &dproc->p_recvmsg;
		memcpy(dmsg, msg, sizeof(struct message));
		dmsg->from = proc->p_endpoint;
		dmsg->to = who;
		// proc flags change
		dproc->p_rts_flags &= (~P_RECVING);
		dproc->p_misc_flags |= P_DELIEVE;
		dproc->uregs->x[0] = 0;
		if(dproc->p_rts_flags == 0) {
			enqueue(dproc);			
		}
	}else{
		// msg copy
		//DMSG("%d:sn send 2\n", proc->p_endpoint);
		proc->p_rts_flags |= (P_SENDING | P_RECVING);
		proc->p_sendto = who;
		dmsg = &proc->p_sendmsg; 	
		memcpy(dmsg, msg, sizeof(struct message));
		dmsg->from = proc->p_endpoint;
		dmsg->to = who;
		// dest p_caller_q link
		if(dproc->p_caller_q == NULL)
			dproc->p_caller_q = proc;
		else{
			tmp = dproc->p_caller_q;
			while(tmp->p_q_link != NULL)
				tmp = tmp->p_q_link;	
			tmp->p_q_link = proc;
		}
		proc->p_q_link = NULL;
		proc->p_getfrom = who;
		proc->p_recvaddr = msg;
		return 0;
	}
	tmp = NULL;
    sproc = proc->p_caller_q;
    while(sproc != NULL) {
        if(who==sproc->p_endpoint)
            break;
        tmp = sproc;
        sproc = sproc->p_q_link;
    }
    if(sproc != NULL) {
        assert((sproc->p_rts_flags & P_SENDING) && sproc->p_sendto==proc->p_endpoint);
        // msg copy
        //DMSG("%d:sn recv 1\n", proc->p_endpoint);
        smsg = &sproc->p_sendmsg;
        memcpy(msg, smsg, sizeof(struct message));
        assert(smsg->from = sproc->p_endpoint);
        // proc flags change
        sproc->p_rts_flags &= (~P_SENDING);
        if(tmp == NULL)
            proc->p_caller_q = sproc->p_q_link;
        else
            tmp->p_q_link = sproc->p_q_link;
        sproc->uregs->x[0] = 0;
        if(sproc->p_rts_flags == 0) {
            enqueue(sproc);
        }
    }else {
        //DMSG("%d:sn recv 2\n", proc->p_endpoint);
        proc->p_rts_flags |= P_RECVING;
        proc->p_getfrom = who;
        proc->p_recvaddr = msg;
    }
	return 0;
}

int sn_notify(int who) {
    struct proc *dproc;
    struct message *dmsg;

    //DMSG("%d:sn send to %d\n", proc->p_endpoint, who);
    if(!(who>=0 && who<NUM_PROCS)) {
        return -1;
    }

    dproc = &procs[who];
    if(dproc->p_rts_flags & P_RECVING) {
        struct message m;
        m.type = M_TYPE_NOTIFY;
        m.u.ts = 2;
        dmsg = &dproc->p_recvmsg;
        memcpy(dmsg, &m, sizeof(struct message));
        dmsg->to = who;
        // proc flags change
        dproc->p_rts_flags &= (~P_RECVING);
        dproc->p_misc_flags |= P_DELIEVE;
        dproc->uregs->x[0] = 0;
        if(dproc->p_rts_flags == 0) {
            enqueue(dproc);
        }
    } else{
        dproc->p_pending = 1;
    }
    //DMSG("SNOW I am here\n");
    return 0;
}
