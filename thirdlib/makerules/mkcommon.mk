
BIOS_66AK2L_app:
	$(MAKE) -fMakefile -C$(dvr_rdk_PATH)/bios6/66ak2l/ 

BIOS_AM5728_app:
	$(MAKE) -fMakefile -C$(dvr_rdk_PATH)/bios6/am5728/ 

BIOS_66AK2L_app_clean:
	$(MAKE) -fMakefile -C$(dvr_rdk_PATH)/bios6/66ak2l/ clean

BIOS_AM5728_app_clean:
	$(MAKE) -fMakefile -C$(dvr_rdk_PATH)/bios6/am5728/ clean

############################################################################
#am5728
am5728_lib:
	$(MAKE) -fMAKEFILE_AM5728.MK -C$(MKCOMMON_PATH) lib

am5728_lib_clean:
	$(MAKE) -fMAKEFILE_AM5728.MK -C$(MKCOMMON_PATH) lib_clean
	
am5728_app:
	$(MAKE) -fMAKEFILE_AM5728.MK -C$(MKCOMMON_PATH) app

am5728_app_clean:
	$(MAKE) -fMAKEFILE_AM5728.MK -C$(MKCOMMON_PATH) app_clean

app_am5728:am5728_app
lib_am5728:am5728_lib

############################################################################

############################################################################
#ubuntu
ubuntu_lib:
	$(MAKE) -fMAKEFILE_UBUNTU.MK -C$(MKCOMMON_PATH) lib

ubuntu_lib_clean:
	$(MAKE) -fMAKEFILE_UBUNTU.MK -C$(MKCOMMON_PATH) lib_clean
	
ubuntu_app:
	$(MAKE) -fMAKEFILE_UBUNTU.MK -C$(MKCOMMON_PATH) app

ubuntu_app_clean:
	$(MAKE) -fMAKEFILE_UBUNTU.MK -C$(MKCOMMON_PATH) app_clean

app_ubuntu:ubuntu_app
lib_ubuntu:ubuntu_lib

############################################################################

############################################################################
#hi3536
hi3536_lib:
	$(MAKE) -fMAKEFILE_hi3536.MK -C$(MKCOMMON_PATH) lib

hi3536_lib_clean:
	$(MAKE) -fMAKEFILE_hi3536.MK -C$(MKCOMMON_PATH) lib_clean
	
hi3536_app:
	$(MAKE) -fMAKEFILE_hi3536.MK -C$(MKCOMMON_PATH) app

hi3536_app_clean:
	$(MAKE) -fMAKEFILE_hi3536.MK -C$(MKCOMMON_PATH) app_clean

app_hi3536:hi3536_app
lib_hi3536:hi3536_lib

############################################################################

############################################################################
#hi3531d
hi3531d_lib:
	$(MAKE) -fMAKEFILE_hi3531d.MK -C$(MKCOMMON_PATH) lib

hi3531d_lib_clean:
	$(MAKE) -fMAKEFILE_hi3531d.MK -C$(MKCOMMON_PATH) lib_clean
	
hi3531d_app:
	$(MAKE) -fMAKEFILE_hi3531d.MK -C$(MKCOMMON_PATH) app

hi3531d_app_clean:
	$(MAKE) -fMAKEFILE_hi3531d.MK -C$(MKCOMMON_PATH) app_clean

app_hi3531d:hi3531d_app
lib_hi3531d:hi3531d_lib

############################################################################

############################################################################
#hi3531a
hi3531a_lib:
	$(MAKE) -fMAKEFILE_hi3531a.MK -C$(MKCOMMON_PATH) lib

hi3531a_lib_clean:
	$(MAKE) -fMAKEFILE_hi3531a.MK -C$(MKCOMMON_PATH) lib_clean
	
hi3531a_app:
	$(MAKE) -fMAKEFILE_hi3531a.MK -C$(MKCOMMON_PATH) app

hi3531a_app_clean:
	$(MAKE) -fMAKEFILE_hi3531a.MK -C$(MKCOMMON_PATH) app_clean

app_hi3531a:hi3531a_app
lib_hi3531a:hi3531a_lib

############################################################################

############################################################################
#66ak2l
k2l-evm_lib:
	$(MAKE) -fMAKEFILE_66ak2l.MK -C$(MKCOMMON_PATH) lib

k2l-evm_lib_clean:
	$(MAKE) -fMAKEFILE_66ak2l.MK -C$(MKCOMMON_PATH) lib_clean
	
k2l-evm_app:
	$(MAKE) -fMAKEFILE_66ak2l.MK -C$(MKCOMMON_PATH) app

k2l-evm_app_clean:
	$(MAKE) -fMAKEFILE_66ak2l.MK -C$(MKCOMMON_PATH) app_clean

app_k2l-evm:k2l-evm_app
lib_k2l-evm:k2l-evm_lib

############################################################################

############################################################################
#A113D
A113D_lib:
	$(MAKE) -fMAKEFILE_A113D.MK -C$(MKCOMMON_PATH) lib

A113D_lib_clean:
	$(MAKE) -fMAKEFILE_A113D.MK -C$(MKCOMMON_PATH) lib_clean
	
A113D_app:
	$(MAKE) -fMAKEFILE_A113D.MK -C$(MKCOMMON_PATH) app

A113D_app_clean:
	$(MAKE) -fMAKEFILE_A113D.MK -C$(MKCOMMON_PATH) app_clean

app_A113D:A113D_app
lib_A113D:A113D_lib
############################################################################
