
dvr_rdk_PATH = $(CURDIR)
COMPILE_RES_PATH=$(dvr_rdk_PATH)/compile_res/
interface_PATH = $(dvr_rdk_PATH)/interface/
jni_PATH=$(dvr_rdk_PATH)/jni
THIRD_LIB_PATH = $(dvr_rdk_PATH)/pre_build/
BUILD_PATH = $(dvr_rdk_PATH)/build/
#模块名，与生成库及执行文件名有关
MODULE_NAME = ui
MODULE_NAME_CGI = ui_CGI

#接口头文件目录
INTERFACE_INCLUDE = $(interface_PATH)/src/$(MODULE_NAME).h
INTERFACE_API_INCLUDE = $(interface_PATH)/src/$(MODULE_NAME)Api.h

MKCOMMON_PATH = $(dvr_rdk_PATH)/makerules/
export MKCOMMON_PATH
export THIRD_LIB_PATH


export BUILD_PATH
export dvr_rdk_PATH
export COMPILE_RES_PATH
export interface_PATH
export MODULE_NAME 
export INTERFACE_INCLUDE
export INTERFACE_API_INCLUDE
export MODULE_NAME_CGI


AUD_API_SDK_NAME = JGD215
AUD_API_SDK_PATH = $(dvr_rdk_PATH)/interface/
export AUD_API_SDK_NAME 
export AUD_API_SDK_PATH

TAR_SUFF_GZ = .tar.gz
TAR_SUFF_BZ = .tar.bz2
TAR_SUFF_XZ = .tar.xz
export TAR_SUFF_GZ
export TAR_SUFF_BZ
export TAR_SUFF_XZ

pre_BUILD_PATH := $(dvr_rdk_PATH)/pre_build/
export pre_BUILD_PATH

###############################################
#common third lib
thrpartylib_PATH := $(dvr_rdk_PATH)/thirdlib/
export thrpartylib_PATH


#ortp 
ORTP = ortp
ORTP_VERSION = $(ORTP)-0.25.0
export ORTP_VERSION

#curl 
LIBCURL = curl
LIBCURL_VERSION = $(LIBCURL)
export LIBCURL_VERSION

#zlib 
LIBZLIB = zlib
LIBZLIB_VERSION = $(LIBZLIB)-1.2.13
export LIBZLIB_VERSION

#cJSON 
CJSONLIB = cJSON
CJSONLIB_VERSION = $(CJSONLIB)-master
export CJSONLIB_VERSION

#FFMPEG 
FFMPEGLIB = ffmpeg
FFMPEGLIB_VERSION = $(FFMPEGLIB)-6.0
export FFMPEGLIB_VERSION

#lighttpd 
LIGHTTPD = lighttpd
LIGHTTPD_VERSION = $(LIGHTTPD)-1.4.71
export LIGHTTPD_VERSION

#pcre 
PCRE = pcre2
PCRE_VERSION = $(PCRE)-10.42
export PCRE_VERSION

#uv 
UV = uv
UV_VERSION = lib$(UV)
export UV_VERSION

#cgic 
CGIC = cgic
CGIC_VERSION = $(CGIC)
export CGIC_VERSION

#mosquitto 
MOSQUITTO = mosquitto
MOSQUITTO_VERSION = $(MOSQUITTO)-2.0.6
export MOSQUITTO_VERSION


#lvgl 
LVGL = lvgl
LVGL_VERSION = $(LVGL)
export LVGL_VERSION

#openssl 
OPENSSL = openssl
OPENSSL_VERSION = $(OPENSSL)-3.0.13
export OPENSSL_VERSION


###############################################

#ubuntu
api_ubuntu:
	# git log --oneline|awk 'NR==1{print}'> scripts/gitversion
	$(MAKE) -fMAKEFILE_UBUNTU.MK -C$(dvr_rdk_PATH)/makerules JGDSDK_app
package_ubuntu:
	$(MAKE) -fMAKEFILE_UBUNTU.MK -C$(dvr_rdk_PATH)/makerules package

api_jgd215:
	# git log --oneline|awk 'NR==1{print}'> scripts/gitversion
	$(MAKE) -fMAKEFILE_JGD215.MK -C$(dvr_rdk_PATH)/makerules JGDSDK_app
package_jgd215:
	$(MAKE) -fMAKEFILE_JGD215.MK -C$(dvr_rdk_PATH)/makerules package

copy:
	scp -rv build/IPKG_BUILD/root/jgd215/app_$(MODULE_NAME).out build/IPKG_BUILD/root/jgd215/lighttpd root@192.168.0.188:/root/jgd215/

httptest:
	gcc interface/src/http.cpp -I//home/zeng/share/jgd215/JGD215/pre_build/ubuntu/inc/ -I//home/zeng/share/jgd215/JGD215/pre_build/ubuntu/inc/cjson -I//home/zeng/share/jgd215/JGD215/interface/src/ -L//home/zeng/share/jgd215/JGD215/pre_build/ubuntu/lib/ -lcjson -lcurl -lz -lpthread -DAAA

armhttptest:
	arm-linux-gnueabihf-gcc interface/src/http.cpp -I//home/zeng/share/jgd215/JGD215/pre_build/JGD215/inc/ -I//home/zeng/share/jgd215/JGD215/pre_build/JGD215/inc/cjson -I//home/zeng/share/jgd215/JGD215/interface/src/ -L//home/zeng/share/jgd215/JGD215/pre_build/JGD215/lib/ -lcjson -lcurl -lz -lpthread -DAAA


include $(dvr_rdk_PATH)/makerules/thirdlib.mk
include $(dvr_rdk_PATH)/makerules/package.mk
