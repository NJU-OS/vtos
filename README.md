# vtos

## 开发指导
- [参与 NJU-OS 项目的开发（一）](http://www.chenup.top/2018/05/14/1-njuos-guide/)
- [参与 NJU-OS 项目的开发（二）](http://www.chenup.top/2018/05/17/2-upstream-repo/)

## 开发进度
- u-boot-nju/log-for-dev.md
- vtos/log-for-dev.md

## 编译 vtos
下载编译需要的工具包 [toolchains](https://github.com/NJU-OS/toolchains)
```
>>> make –f toolchains.mk
```

编译
```
>>> make
```

## 编译过程中可能遇到的问题及解决方案（在 ubuntu 16.04 下测试成功）
- `ImportError: No module named Crypto.PublicKey`
```
# 下载 get-pip.py
>>> curl https://bootstrap.pypa.io/get-pip.py -o get-pip.py

# 安装 pip
>>> sudo python get-pip.py

# 测试
>>> pip -v

#安装 aptitude
>>> sudo apt-get install aptitude

# 用 aptitude 安装 python-dev
>>> sudo aptitude install python-dev

# 安装 pycrypto
>>> sudo pip install pycrypto
```

- `ImportError: No module named wand.image`
```
>>> sudo apt-get install libmagickwand-dev
>>> sudo pip install Wand
```
