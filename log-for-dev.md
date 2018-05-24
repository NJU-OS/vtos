# vtos 开发记录

规则：

- `*` 修改文件
- `+` 增加文件
- `-` 删除文件

样例：

---
#### 更新时间: 
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
#### 更新时间: 
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
