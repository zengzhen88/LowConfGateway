BOARD_ARCH = Dm81xx
CORE_OS    = Linux

include $(dvr_rdk_PATH)/makerules/common_header_a8.mk

LIBS += $(LIB_DIR)/lib$(MODULE_NAME_CGI)Intf_Demo.a
LIBS += $(LIB_DIR)/lib$(MODULE_NAME)Intf.a
LIBS += -ldl

DEFINE += -DUNIX
DEFINE += -D__STDC_CONSTANT_MACROS
DEFINE += -DMAINUSE
include $(dvr_rdk_PATH)/makerules/common_footer_a8.mk


