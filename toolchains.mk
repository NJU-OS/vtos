# restrict only works on bash
SHELL = /bin/bash

################################################################################
# Toolchains
################################################################################
ROOT				?= $(CURDIR)/..
TOOLCHAIN_ROOT 			?= $(ROOT)/toolchains

EXIST_YES 	?= YES
EXIST_NO	?= NO
TOOLCHAIN_REPO ?= https://github.com/NJU-OS/toolchains.git

AARCH32_PATH 			?= $(TOOLCHAIN_ROOT)/aarch32
AARCH32_CROSS_COMPILE 		?= $(AARCH32_PATH)/bin/arm-linux-gnueabihf-
AARCH32_GCC_VERSION 		?= gcc-linaro-6.2.1-2016.11-x86_64_arm-linux-gnueabihf
SRC_AARCH32_GCC 		?= http://releases.linaro.org/components/toolchain/binaries/6.2-2016.11/arm-linux-gnueabihf/${AARCH32_GCC_VERSION}.tar.xz

AARCH64_PATH 			?= $(TOOLCHAIN_ROOT)/aarch64
AARCH64_CROSS_COMPILE 		?= $(AARCH64_PATH)/bin/aarch64-linux-gnu-
AARCH64_GCC_VERSION 		?= gcc-linaro-6.2.1-2016.11-x86_64_aarch64-linux-gnu
SRC_AARCH64_GCC 		?= http://releases.linaro.org/components/toolchain/binaries/6.2-2016.11/aarch64-linux-gnu/${AARCH64_GCC_VERSION}.tar.xz

# Due to relocation error on the 96board edk forest, let's keep the old
# toolchain for a while.
LEGACY_AARCH64_PATH             ?= $(TOOLCHAIN_ROOT)/aarch64-legacy
LEGACY_AARCH64_CROSS_COMPILE    ?= $(LEGACY_AARCH64_PATH)/bin/aarch64-linux-gnu-
LEGACY_AARCH64_GCC_VERSION      ?= gcc-linaro-aarch64-linux-gnu-4.9-2014.08_linux
LEGACY_SRC_AARCH64_GCC          ?= http://releases.linaro.org/archive/14.08/components/toolchain/binaries/${LEGACY_AARCH64_GCC_VERSION}.tar.xz

# Download toolchain macro for saving some repetition
# $(1) is $AARCH.._PATH		: i.e., path to the destination
# $(2) is $SRC_AARCH.._GCC	: is the downloaded tar.gz file
# $(3) is $.._GCC_VERSION	: the name of the file to download
define dltc
	@if [ ! -d "$(1)" ]; then \
		mkdir -p $(1); \
		tar xf $(TOOLCHAIN_ROOT)/$(3).tar.xz -C $(1) --strip-components=1; \
	fi
endef

# check whether we have already downloaded the toolchains
TOOLCHAIN_FILE = ${AARCH32_GCC_VERSION}.tar.gz
EXIST = $(shell if [ -f $(TOOLCHAIN_FILE) ]; then echo $(EXIST_YES); else echo $(EXIST_NO); fi)


.PHONY: toolchains
toolchains: exist aarch32 aarch64 aarch64-legacy

.PHONY: aarch32
aarch32:
	$(call dltc,$(AARCH32_PATH),$(SRC_AARCH32_GCC),$(AARCH32_GCC_VERSION))

.PHONY: aarch64
aarch64:
	$(call dltc,$(AARCH64_PATH),$(SRC_AARCH64_GCC),$(AARCH64_GCC_VERSION))

.PHONY: aarch64-legacy
aarch64-legacy:
	$(call dltc,$(LEGACY_AARCH64_PATH),$(LEGACY_SRC_AARCH64_GCC),$(LEGACY_AARCH64_GCC_VERSION))

.PHONY: exist
exist:
	@set -e
	@echo "repo: $(TOOLCHAIN_REPO) not exist, so will downloading..."
	@if [ "$(EXIST)" == "NO" ]; then \
		pushd $(ROOT); \
		git clone $(TOOLCHAIN_REPO); \
		popd ;\
	fi
