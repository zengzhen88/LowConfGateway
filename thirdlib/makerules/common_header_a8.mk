# (c) Texas Instruments

ifndef $(COMMON_HEADER_MK)
COMMON_HEADER_MK = 1

ifeq ($(CONFIG),)
LIB_DIR=$(LIB_BASE_DIR)
else
LIB_DIR=$(LIB_BASE_DIR)/$(CONFIG)
endif

ifeq ($(PLATFORM), ubuntu)
CC=$(CODEGEN_PREFIX)gcc
XX=$(CODEGEN_PREFIX)g++
AR=$(CODEGEN_PREFIX)ar
LD=$(CODEGEN_PREFIX)gcc
CP=cp
# CC=$(CODEGEN_PREFIX)gcc-4.8
# XX=$(CODEGEN_PREFIX)g++-4.8
# AR=$(CODEGEN_PREFIX)gcc-ar-4.8
# LD=$(CODEGEN_PREFIX)gcc-4.8
else
CC=$(CODEGEN_PREFIX)gcc
XX=$(CODEGEN_PREFIX)g++
AR=$(CODEGEN_PREFIX)ar
LD=$(CODEGEN_PREFIX)gcc
CP=cp
endif

LIB_BASE_DIR=$(COMPILE_RES_PATH)/$(PLATFORM)/app/lib
OBJ_BASE_DIR=$(COMPILE_RES_PATH)/$(PLATFORM)/app/obj
EXE_BASE_DIR=$(COMPILE_RES_PATH)/$(PLATFORM)/app/bin
INC_BASE_DIR=$(COMPILE_RES_PATH)/$(PLATFORM)/app/inc

CC_OPTS=-c -Wall -Warray-bounds -Werror 

ifeq ($(TREAT_WARNINGS_AS_ERROR), yes)

ifeq ($(NO_WERROR), )
	#CC_OPTS+= -Werror
else
	CC_OPTS += -Wno-deprecated 
endif

endif

ifeq ($(CONFIG), debug)

# CC_OPTS+=-g -mcpu=cortex-a8 -mfpu=neon -mfloat-abi=softfp
OPTI_OPTS=
DEFINE=-DDEBUG

else

#CC_OPTS+=-mcpu=cortex-a8 -mfpu=neon -mfloat-abi=softfp
# OPTI_OPTS=-O3
OPTI_OPTS=
DEFINE=

endif

AR_OPTS=-rc
LD_OPTS+=-lpthread -lm -lstdc++ -rdynamic -lrt

S_OPTS+=

DEFINE +=
FILES=$(subst ./, , $(foreach dir,.,$(wildcard $(dir)/*.c)) )

vpath %.a $(LIB_DIR)

endif # ifndef $(COMMON_HEADER_MK)

