#ifndef VTOS_KERNEL_MSG_H
#define VTOS_KERNEL_MSG_H

int syscall_send(struct proc *proc);
int syscall_receive(struct proc *proc);
int syscall_sendrec(struct proc *proc);

#endif // VTOS_KERNEL_MSG_H
