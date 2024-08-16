BOARD_ARCH = Dm81xx
CORE_OS    = Linux

include $(dvr_rdk_PATH)/makerules/common_header_a8.mk

INCLUDE += -I$(interface_PATH)/src/
INCLUDE += -I$(api_PATH)/src/
INCLUDE += -I$(interface_PATH)/src/gener/tska/inc/
INCLUDE += -I$(interface_PATH)/src/gener/rbtree/inc/
INCLUDE += -I$(interface_PATH)/src/gener/rbtree/src/
INCLUDE += -I$(interface_PATH)/src/gener/list/inc/
INCLUDE += -I$(THIRD_LIB_PATH)/$(PLATFORM)/inc/
INCLUDE += -I$(THIRD_LIB_PATH)/$(PLATFORM)/inc/cjson
INCLUDE += -I$(interface_PATH)/src/gener/msgq/inc

# LIBS += $(THIRD_LIB_PATH)/$(PLATFORM)/lib/libcurl.a
# LIBS += $(THIRD_LIB_PATH)/$(PLATFORM)/lib/libcjson.a

# INCLUDE += -I$(src_package_PATH)/webrtc/compile_res/$(PLATFORM)/app/inc
# INCLUDE += -I$(dvr_rdk_PATH)/pre_build/$(PLATFORM)/inc

#LD_OPTS+=-lg722Intf -lm -lstdc++ -rdynamic -lrt

SRCDIR += $(interface_PATH)/src/gener/tska/src
SRCDIR += $(interface_PATH)/src/gener/rbtree/src
# SRCDIR += $(interface_PATH)/src/gener/msgq/src
DEFINE += -DUNIX
DEFINE += -D__STDC_CONSTANT_MACROS
include $(dvr_rdk_PATH)/makerules/common_footer_a8.mk


