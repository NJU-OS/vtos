
export V?=0

LDPATH ?= /media/sdb8/project/toolchains/aarch64/aarch64-linux-gnu/libc/lib

.PHONY: all
all:
	$(MAKE) -C ta CROSS_COMPILE="$(TA_CROSS_COMPILE)"

.PHONY: clean
clean:
	$(MAKE) -C ta clean
