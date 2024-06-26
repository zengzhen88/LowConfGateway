# (c) Texas Instruments

ifndef $(COMMON_FOOTER_MK)
COMMON_FOOTER_MK = 1

LD_OPTS += -L$(LIB_DIR)

ifeq ($(SRCDIR), )
	FILES     +=$(subst ./,,$(foreach dir,.,$(wildcard $(dir)/*.c)) )
	CPP_FILES +=$(subst ./,,$(foreach dir,.,$(wildcard $(dir)/*.cpp)) )
	CXX_FILES +=$(subst ./,,$(foreach dir,.,$(wildcard $(dir)/*.cxx)) )
	SS_FILES +=$(subst ./,,$(foreach dir,.,$(wildcard $(dir)/*.S)) )
else
	SRC_PATH  :=$(filter-out ./,$(SRCDIR))
	SRC_PATH  := . $(SRC_PATH)

	FILES     +=$(subst ./,,$(foreach dir,$(SRC_PATH),$(wildcard $(dir)/*.c)) )
	CPP_FILES +=$(subst ./,,$(foreach dir,$(SRC_PATH),$(wildcard $(dir)/*.cpp)) )
	CXX_FILES +=$(subst ./,,$(foreach dir,$(SRC_PATH),$(wildcard $(dir)/*.cxx)) )
	SS_FILES +=$(subst ./,,$(foreach dir,$(SRC_PATH),$(wildcard $(dir)/*.S)) )
endif

OBJS     :=$(subst .c,.o, $(FILES) )
CPP_OBJS :=$(subst .cpp,.o, $(CPP_FILES) )
CXX_OBJS :=$(subst .cxx,.o, $(CXX_FILES) )
SS_OBJS :=$(subst .S,.o, $(SS_FILES) )


OBJ_DIR=$(OBJ_BASE_DIR)/$(MODULE)/$(CONFIG)

OBJ_DIR_FILES = $(OBJ_DIR)/*.o

vpath %.o  $(OBJ_DIR)

LIB = $(MODULE).a

EXE = $(EXE_BASE_DIR)/$(CONFIG)/$(MODULE).out

SO_LIB= $(EXE_BASE_DIR)/$(CONFIG)/$(MODULE).so

.c.o:
	@echo \# $(MODULE): $(PLATFORM): Compiling $<
	$(CC) $(CC_OPTS) $(OPTI_OPTS) $(DEFINE) $(INCLUDE) -o$(OBJ_DIR)/$(notdir $@) $<

.cpp.o:
	@echo \# $(MODULE): $(PLATFORM): Compiling $<
	$(XX) $(CC_OPTS) $(OPTI_OPTS) $(DEFINE) $(INCLUDE) -o$(OBJ_DIR)/$(notdir $@) $<

Sdp%.o:Sdp%.cxx
	@echo \# $(MODULE): $(PLATFORM): Compiling $<
	$(XX) $(CC_OPTS) $(OPTI_OPTS) $(DEFINE) $(INCLUDE) -o$(OBJ_DIR)/$(notdir $@) $<

.S.o:
	@echo \# $(MODULE): $(PLATFORM): Compiling $<
	$(CC) $(CC_OPTS) $(OPTI_OPTS) $(DEFINE) $(INCLUDE) -o$(OBJ_DIR)/$(notdir $@) $<

lib : $(LIB_DIR)/$(LIB)

$(LIB_DIR)/$(LIB) : $(OBJS) $(CPP_OBJS) ${CXX_OBJS} $(SS_OBJS)
	@echo cxx objs:${CXX_OBJS}
	@echo \# $(MODULE): $(PLATFORM): Creating archive $(LIB)
	@echo \# $(OBJS) $(CPP_OBJS) ${CXX_OBJS} $(SS_OBJS)
	$(AR)	$(AR_OPTS) $(LIB_DIR)/$(LIB) $(OBJ_DIR_FILES) $(MY_LIBS)

obj: $(OBJS) $(CPP_OBJS) ${CXX_OBJS} $(SS_OBJS)

clean:
	@echo \# $(MODULE): $(PLATFORM): Deleting temporary files
	-rm -f $(LIB_DIR)/$(LIB)
	-rm -f $(OBJ_DIR)/*.*

depend:
#	@echo \# $(MODULE): $(PLATFORM): Making Directories, if not already created
	-mkdir -p $(LIB_DIR)
	-mkdir -p $(OBJ_DIR)
	-mkdir -p $(EXE_BASE_DIR)
	-mkdir -p $(INC_BASE_DIR)
	@echo \# $(MODULE): $(PLATFORM): Building dependancies
	@echo $(FILES)
	@echo $(OBJS)
	$(CC) $(CC_OPTS)	$(DEFINE) $(INCLUDE) $(FILES) $(CPP_FILES) ${CXX_FILES} $(SS_FILES) -M > MAKEFILE.DEPEND

so:
	@echo \# $(MODULE): $(PLATFORM): Linking to .so
	$(LD) $(LD_OPTS) -shared -o $(SO_LIB) $(OBJ_DIR_FILES) $(LIBS)
	@echo \# Final shared library $(SO_LIB) !!!
	@echo \#
exe:
	@echo \# $(MODULE): $(PLATFORM): Linking
	@echo \# $(LIBS) $(LD_OPTS)
	$(LD) $(LIBS) $(LD_OPTS)  -o$(EXE)
	@echo \# $(CP) -rf $(INTERFACE_INCLUDE) $(INC_BASE_DIR)
	$(CP) -rf $(INTERFACE_INCLUDE) $(INC_BASE_DIR)
	@echo \# Final executable $(EXE) !!!
	@echo \#

install:


-include MAKEFILE.DEPEND

endif # ifndef $(COMMON_FOOTER_MK)

