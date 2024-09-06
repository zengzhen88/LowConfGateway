
include $(dvr_rdk_PATH)/makerules/common_header_a8.mk

INCLUDE+= $(OSA_KERMOD_INC) $(DEVICES_INC)
LD_OPTS += -lrt
ifeq ($(PLATFORM), ubuntu)
DEFINE += -D MAKE_CHIP_INTEL
endif
include $(dvr_rdk_PATH)/makerules/common_footer_a8.mk


