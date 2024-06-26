CODEGEN_PREFIX=/opt/amlogic/toolchain/gcc/linux-x86/arm/gcc-linaro-6.3.1-2017.02-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-
PLATFORM=A113D

CODEGEN_HOST=arm-linux-gnueabihf

LD_OPTS += 
CC_OPTS += -mfpu=neon-vfpv4# -Werror -g
DEFINE +=
ARM_PLATFORM = 1
USE_LIB_LS_JFFT_ALG_V18 = 1

export LD_OPTS
export CC_OPTS 
export DEFINE
export ARM_PLATFORM

include $(dvr_rdk_PATH)/makerules/MAKEFILE_COMMON.MK
