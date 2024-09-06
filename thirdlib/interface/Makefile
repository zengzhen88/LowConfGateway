app:
	@echo "compile app for test dvr_rdk!!"
	$(MAKE) -fMakefile libs TARGET=depend
	@echo "compile app for test dvr_rdk!!"
	$(MAKE) -fMakefile libs
	$(MAKE) -fMakefile applibs TARGET=depend
	$(MAKE) -fMakefile applibs
	$(MAKE) -fMakefile apps TARGET=exe

lib:
	@echo "compile app for test dvr_rdk!!"
	$(MAKE) -fMakefile libs TARGET=depend
	@echo "compile app for test dvr_rdk!!"
	$(MAKE) -fMakefile libs

applibs:
	@echo "compile app for libs!!"
	$(MAKE) -fMakefile -C$(interface_PATH)/app/ MODULE=lib$(MODULE_NAME)_Demo_Intf $(TARGET)
	$(MAKE) -fMakefile -C$(interface_PATH)/cgi/ MODULE=lib$(MODULE_NAME_CGI)_Demo_Intf $(TARGET)

apps:
	@echo "compile app for test apps!!"
	$(MAKE) -fMAKEFILE.MK -C$(interface_PATH)/app/ MODULE=app_$(MODULE_NAME) $(TARGET)
	$(MAKE) -fMAKEFILE.MK -C$(interface_PATH)/cgi/ MODULE=app_$(MODULE_NAME_CGI) $(TARGET)

libs:
	@echo "compile app for libs!!"
	$(MAKE) -fMakefile -C$(interface_PATH)/src/ MODULE=lib$(MODULE_NAME)Intf $(TARGET)
	$(MAKE) -fMakefile -C$(interface_PATH)/app/ MODULE=lib$(MODULE_NAME)Intf_Demo $(TARGET)
	$(MAKE) -fMakefile -C$(interface_PATH)/cgi/ MODULE=lib$(MODULE_NAME_CGI)Intf_Demo $(TARGET)

clean:
	$(MAKE) -fMakefile libs TARGET=clean

all: clean dvr_rdk

.PHONY: app lib apps libs clean all

