
# vtos boot-steps
## start boot
file: vtos/core/arch/arm/kernel/kern.ld.S
这个文件描述了整个vtos的链接地址，其中
ENTRY（_start)表示，编译生成的内核文件从_start文件开始执行。

_start函数
file: vtos/core/arch/arm/kernel/generic_entry_a64.S
	1.初始化MMU
	2.执行generic_boot_init_primary函数
	

generic_boot_init_primary函数
|
|
+--> init_primary_helper
		|
        +--thread_set_exceptions
        +--init_runtime
        +--thread_init_vbar
        +--main_init_gic
        +--init_generic_timer
        +--init_vfp_nsec
        +--init_teecore
        +--proc_init
        +--thread_unmask_exceptions
