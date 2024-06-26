#编译器
CODEGEN_PREFIX=/opt/tisdk-am5728/linux-devkit/sysroots/x86_64-arago-linux/usr/bin/arm-linux-gnueabihf-

CODEGEN_HOST=arm-linux-gnueabihf

#对应的平台名字，决定编译文件生成位置
PLATFORM=am5728

#对应编译所对应的编译选项
LD_OPTS+=-mcpu=cortex-a15 -mfpu=neon -mfloat-abi=hard
LD_OPTS += -lz
ARM_PLATFORM = 1

export LD_OPTS
export CC_OPTS 
export ARM_PLATFORM

#默认需要
include $(dvr_rdk_PATH)/makerules/MAKEFILE_COMMON.MK
