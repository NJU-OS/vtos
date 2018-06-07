# vtos 开发记录

规则：

- `*` 修改文件
- `+` 增加文件
- `-` 删除文件

样例：

---
#### 更新时间：
`2018-05-16`

#### 负责人员：
- rex

#### 主要更新内容：
- 控制 `gpio J6` 的 `out` 为高电平，使得连接的发光二极管被关闭，它可以作为内核早期开发的调试

#### 主要涉及的文件：

```
* /common/board_r.c
+ /common/bootvtos.c
- /common/bootm.c
```

## 以下是正文



---
#### 更新时间： 
`2018-05-24`

#### 负责人员：
- snow

#### 主要更新内容：
- 完成类似printf的串口输出函数sn_printf，支持可变长参数
- sn_test函数对sn_printf函数进行了测试，包括%s，%x和%d参数

#### 主要涉及的文件：

```
* /core/arch/arm/kernel/generic_entry_a64.S
* /kernel/proc.h
* /kernel/proc.c
* /lib/libutils/ext/include/trace.h
* /lib/libutee/trace_ext.c
```

---
#### 更新时间： 
`2018-05-25`

#### 负责人员：
- snow

#### 主要更新内容：
- 完成串口输出对DMSG调试接口的支持，可显示DMSG所在函数名和代码行号
- sn_test函数对DMSG进行了测试，包括%s，%x和%d参数

#### 主要涉及的文件：

```
* /core/arch/arm/kernel/generic_entry_a64.S
* /kernel/proc.h
* /kernel/proc.c
* /lib/libutils/ext/trace.c
* /core/arch/arm/kernel/trace_ext.c
```

---
#### 更新时间：
`2018-05-28`

#### 负责人员：
- snow

#### 主要更新内容：
- 完成页表初始化并启用MMU分页，通过串口验证分页后的地址转换正确性
- 在分页启用前后分别调用sn_test函数进行串口输出

#### 主要涉及的文件：

```
* /core/arch/arm/kernel/generic_entry_a64.S
* /kernel/proc.h
* /kernel/proc.c
* /core/arch/arm/mm/core_mmu.c
```

#### 主要更新内容：
- 完成串口输出对DMSG调试接口的支持，可显示DMSG所在函数名和代码行号
- sn_test函数对DMSG进行了测试，包括%s，%x和%d参数

#### 主要涉及的文件：

```
* /core/arch/arm/kernel/generic_entry_a64.S
* /kernel/proc.h
* /kernel/proc.c
* /lib/libutils/ext/trace.c
* /core/arch/arm/kernel/trace_ext.c
```

---
#### 更新时间： 
`2018-05-28`

#### 负责人员：
- snow

#### 主要更新内容：
- 完成页表初始化并启用MMU分页，通过串口验证分页后的地址转换正确性
- 在分页启用前后分别调用sn_test函数进行串口输出

#### 主要涉及的文件：

```
* /core/arch/arm/kernel/generic_boot.c
* /core/arch/arm/mm/tee_mmu.c
* /core/arch/arm/kernel/thread.c
```

---
#### 更新时间： 
`2018-06-06`

#### 负责人员：
- rex

#### 主要更新内容：
- 增加了通用中断处理过程
- 增加了时钟中断

#### 主要涉及的文件：

```
* /core/arch/arm/kernel/generic_boot.c
* /core/arch/arm/include/kernel/thread.h
* /core/arch/arm/include/kernel/generic_boot.h
* /core/arch/arm/plat-vexpress/main.c
* /core/arch/arm/plat-vexpress/platform_config.h
* /core/arch/arm/include/arm64.h
* /core/arch/arm/kernel/thread_a64.S
* /core/arch/arm/kernel/pseudo_ta.c
* /core/tee/tee_cryp_utl.c
* /core/tee/tee_fs_key_manager.c
* /core/arch/arm/tee/entry_std.c
* /core/kernel/interrupt.c
```

