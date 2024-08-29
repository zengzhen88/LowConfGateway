BOARD_ARCH = Dm81xx
CORE_OS    = Linux

include $(dvr_rdk_PATH)/makerules/common_header_a8.mk

INCLUDE += -I$(interface_PATH)/src/
INCLUDE += -I$(api_PATH)/src/
#INCLUDE += -I$(interface_PATH)/src/utils

# INCLUDE += -I$(src_package_PATH)/webrtc/compile_res/$(PLATFORM)/app/inc
INCLUDE += -I$(dvr_rdk_PATH)/pre_build/$(PLATFORM)/inc

#LD_OPTS+=-lg722Intf -lm -lstdc++ -rdynamic -lrt

DEFINE += -DUNIX
DEFINE += -D__STDC_CONSTANT_MACROS
include $(dvr_rdk_PATH)/makerules/common_footer_a8.mk


