CODEGEN_PREFIX=
PLATFORM=ubuntu

# LD_OPTS +=
# CC_OPTS += -g -Werror -g
# DEFINE += -DMAKE_CHIP_INTEL -DTEST
ARM_PLATFORM = 0

# export LD_OPTS
export CC_OPTS 
export PLATFORM
# export DEFINE
export ARM_PLATFORM

include $(dvr_rdk_PATH)/makerules/MAKEFILE_COMMON.MK
