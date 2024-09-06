BOARD_ARCH = Dm81xx
CORE_OS    = Linux

include $(dvr_rdk_PATH)/makerules/common_header_a8.mk

INCLUDE += -I./

DEFINE += -DUNIX
DEFINE += -D__STDC_CONSTANT_MACROS
include $(dvr_rdk_PATH)/makerules/common_footer_a8.mk


