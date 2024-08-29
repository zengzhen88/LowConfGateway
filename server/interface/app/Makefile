BOARD_ARCH = Dm81xx
CORE_OS    = Linux

include $(dvr_rdk_PATH)/makerules/common_header_a8.mk

INCLUDE += -I$(interface_PATH)/src/
INCLUDE += -I$(interface_PATH)/demo
INCLUDE += -I$(api_PATH)/src/
INCLUDE += -I$(dvr_rdk_PATH)/pre_build/$(PLATFORM)/inc

DEFINE += -DUNIX
DEFINE += -D__STDC_CONSTANT_MACROS
DEFINE += -DMAINUSE 
include $(dvr_rdk_PATH)/makerules/common_footer_a8.mk


