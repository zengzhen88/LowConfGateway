#编译器
CODEGEN_PREFIX=/opt/66ak2l/linux-devkit/sysroots/x86_64-arago-linux/usr/bin/arm-linux-gnueabihf-

CODEGEN_HOST=arm-linux-gnueabihf

PLATFORM=k2l-evm

CC_OPTS+=-march=armv7-a -marm -mfpu=neon  -mfloat-abi=hard  # -Werror -g
LD_OPTS += -lz

DEFINE +=
ARM_PLATFORM = 1

export LD_OPTS
export CC_OPTS 
export DEFINE
export ARM_PLATFORM

#默认需要
include $(dvr_rdk_PATH)/makerules/MAKEFILE_COMMON.MK
