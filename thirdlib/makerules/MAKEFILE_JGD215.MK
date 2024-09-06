CODEGEN_PREFIX=/home/zeng/Downloads/gcc-arm-8.2-2018.08-x86_64-arm-linux-gnueabihf/bin/arm-linux-gnueabihf-
PLATFORM=JGD215

CODEGEN_HOST=arm-linux-gnueabihf

LD_OPTS += 
CC_OPTS += 
DEFINE +=
ARM_PLATFORM = 1

export LD_OPTS
export CC_OPTS 
export DEFINE
export ARM_PLATFORM
export CODEGEN_PREFIX
export PLATFORM

include $(dvr_rdk_PATH)/makerules/MAKEFILE_COMMON.MK
