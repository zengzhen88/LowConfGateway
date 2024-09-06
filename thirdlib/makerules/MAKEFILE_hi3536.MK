
CODEGEN_PREFIX=arm-hisiv400-linux-
CODEGEN_HOST=arm-hisiv400-linux

PLATFORM=hi3536

LD_OPTS += -lrt -ldl
CC_OPTS += -march=armv7-a -mfloat-abi=softfp -mfpu=neon-vfpv4 -mno-unaligned-access -fno-aggressive-loop-optimizations  -g
DEFINE += 
ARM_PLATFORM = 1

export LD_OPTS
export CC_OPTS 
export DEFINE
export ARM_PLATFORM


include $(dvr_rdk_PATH)/makerules/MAKEFILE_COMMON.MK
