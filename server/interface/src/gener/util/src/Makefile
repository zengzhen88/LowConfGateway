BOARD_ARCH = Dm81xx
CORE_OS    = Linux

include $(dvr_rdk_PATH)/makerules/common_header_a8.mk

INCLUDE += -I$(interface_PATH)/src/util/inc/
INCLUDE += -I$(interface_PATH)/src/util/src/
INCLUDE += -I$(GENER_PATH)/interface/src/tska/inc/
INCLUDE += -I$(LIST_INCLUDE)
INCLUDE += -I$(RBTREE_INCLUDE)

DEFINE += -DUNIX
DEFINE += -D__STDC_CONSTANT_MACROS
include $(dvr_rdk_PATH)/makerules/common_footer_a8.mk


