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



