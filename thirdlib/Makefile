
dvr_rdk_PATH = $(CURDIR)
COMPILE_RES_PATH=$(dvr_rdk_PATH)/compile_res/
interface_PATH = $(dvr_rdk_PATH)/interface/
jni_PATH=$(dvr_rdk_PATH)/jni
THIRD_LIB_PATH = $(dvr_rdk_PATH)/pre_build/
BUILD_PATH = $(dvr_rdk_PATH)/build/
#模块名，与生成库及执行文件名有关
MODULE_NAME = gateway
MODULE_NAME_CGI = gateway_CGI

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

#openssl 
OPENSSL = openssl
OPENSSL_VERSION = $(OPENSSL)-3.0.13
export OPENSSL_VERSION


###############################################

# #ubuntu
# api_ubuntu:
	# # git log --oneline|awk 'NR==1{print}'> scripts/gitversion
	# $(MAKE) -fMAKEFILE_UBUNTU.MK -C$(dvr_rdk_PATH)/makerules JGDSDK_app
# package_ubuntu:
	# $(MAKE) -fMAKEFILE_UBUNTU.MK -C$(dvr_rdk_PATH)/makerules package

# api_jgd215:
	# # git log --oneline|awk 'NR==1{print}'> scripts/gitversion
	# $(MAKE) -fMAKEFILE_JGD215.MK -C$(dvr_rdk_PATH)/makerules JGDSDK_app
# package_jgd215:
	# $(MAKE) -fMAKEFILE_JGD215.MK -C$(dvr_rdk_PATH)/makerules package

######################################################
#ubuntu
#curl_ubuntu:
#	$(MAKE) -fMAKEFILE_UBUNTU.MK -C$(dvr_rdk_PATH)/makerules curl
#zlib_ubuntu:
#	$(MAKE) -fMAKEFILE_UBUNTU.MK -C$(dvr_rdk_PATH)/makerules zlib
json_ubuntu_release:
	$(MAKE) -fMAKEFILE_UBUNTU.MK -C$(dvr_rdk_PATH)/makerules json_release
json_ubuntu_debug:
	$(MAKE) -fMAKEFILE_UBUNTU.MK -C$(dvr_rdk_PATH)/makerules json_debug
#ffmpeg_ubuntu:
#	$(MAKE) -fMAKEFILE_UBUNTU.MK -C$(dvr_rdk_PATH)/makerules ffmpegs
#lighttpd_ubuntu:
#	$(MAKE) -fMAKEFILE_UBUNTU.MK -C$(dvr_rdk_PATH)/makerules lighttpd
#pcre_ubuntu:
#	$(MAKE) -fMAKEFILE_UBUNTU.MK -C$(dvr_rdk_PATH)/makerules pcre
#mosquitto_ubuntu:
#	$(MAKE) -fMAKEFILE_UBUNTU.MK -C$(dvr_rdk_PATH)/makerules mosquitto
openssl_ubuntu_release:
	$(MAKE) -fMAKEFILE_UBUNTU.MK -C$(dvr_rdk_PATH)/makerules openssl_release
openssl_ubuntu_debug:
	$(MAKE) -fMAKEFILE_UBUNTU.MK -C$(dvr_rdk_PATH)/makerules openssl_debug

#curl_ubuntu_clean:
#	$(MAKE) -fMAKEFILE_UBUNTU.MK -C$(dvr_rdk_PATH)/makerules curl_clean
#zlib_ubuntu_clean:
#	$(MAKE) -fMAKEFILE_UBUNTU.MK -C$(dvr_rdk_PATH)/makerules zlib_clean
json_ubuntu_clean:
	$(AMKE) -fMAKEFILE_UBUNTU.MK -C$(dvr_rdk_PATH)/makerules json_clean
#ffmpeg_ubuntu_clean:
#	$(AMKE) -fMAKEFILE_UBUNTU.MK -C$(dvr_rdk_PATH)/makerules ffmpeg_clean
#lighttpd_ubuntu_clean:
#	$(MAKE) -fMAKEFILE_UBUNTU.MK -C$(dvr_rdk_PATH)/makerules lighttpd_clean
#pcre_ubuntu_clean:
#	$(MAKE) -fMAKEFILE_UBUNTU.MK -C$(dvr_rdk_PATH)/makerules pcre_clean
#mosquitto_ubuntu_clean:
#	$(MAKE) -fMAKEFILE_UBUNTU.MK -C$(dvr_rdk_PATH)/makerules mosquitto_clean
openssl_ubuntu_clean:
	$(MAKE) -fMAKEFILE_UBUNTU.MK -C$(dvr_rdk_PATH)/makerules openssl_clean

# thirdlib_ubuntu:  json_ubuntu ffmpeg_ubuntu 
thirdlib_ubuntu_release: json_ubuntu_release openssl_ubuntu_release  
thirdlib_ubuntu_debug: json_ubuntu_debug openssl_ubuntu_debug  
thirdlib_ubuntu_clean:  json_ubuntu_clean ffmpeg_ubuntu_clean openssl_ubuntu_clean  mosquitto_ubuntu

######################################################
######################################################
#windows
#curl_windows_release:
#	$(MAKE) -fMAKEFILE_WINDOWS.MK -C$(dvr_rdk_PATH)/makerules curl_release
#zlib_windows_release:
#	$(MAKE) -fMAKEFILE_WINDOWS.MK -C$(dvr_rdk_PATH)/makerules zlib_release
json_windows_release:
	$(MAKE) -fMAKEFILE_WINDOWS.MK -C$(dvr_rdk_PATH)/makerules json_release
#ffmpeg_windows_release:
#	$(MAKE) -fMAKEFILE_WINDOWS.MK -C$(dvr_rdk_PATH)/makerules ffmpegs_release
#lighttpd_windows_release:
#	$(MAKE) -fMAKEFILE_WINDOWS.MK -C$(dvr_rdk_PATH)/makerules lighttpd_release
#pcre_windows_release:
#	$(MAKE) -fMAKEFILE_WINDOWS.MK -C$(dvr_rdk_PATH)/makerules pcre_release
#mosquitto_windows_release:
#	$(MAKE) -fMAKEFILE_WINDOWS.MK -C$(dvr_rdk_PATH)/makerules mosquitto_release
openssl_windows_release:
	$(MAKE) -fMAKEFILE_WINDOWS.MK -C$(dvr_rdk_PATH)/makerules openssl_release

#curl_windows_debug:
#	$(MAKE) -fMAKEFILE_WINDOWS.MK -C$(dvr_rdk_PATH)/makerules curl_debug
#zlib_windows_debug:
#	$(MAKE) -fMAKEFILE_WINDOWS.MK -C$(dvr_rdk_PATH)/makerules zlib_debug
json_windows_debug:
	$(MAKE) -fMAKEFILE_WINDOWS.MK -C$(dvr_rdk_PATH)/makerules json_debug
#ffmpeg_windows_debug:
#	$(MAKE) -fMAKEFILE_WINDOWS.MK -C$(dvr_rdk_PATH)/makerules ffmpegs_debug
#lighttpd_windows_debug:
#	$(MAKE) -fMAKEFILE_WINDOWS.MK -C$(dvr_rdk_PATH)/makerules lighttpd_debug
#pcre_windows_debug:
#	$(MAKE) -fMAKEFILE_WINDOWS.MK -C$(dvr_rdk_PATH)/makerules pcre_debug
#mosquitto_windows_debug:
#	$(MAKE) -fMAKEFILE_WINDOWS.MK -C$(dvr_rdk_PATH)/makerules mosquitto_debug
openssl_windows_debug:
	$(MAKE) -fMAKEFILE_WINDOWS.MK -C$(dvr_rdk_PATH)/makerules openssl_debug

#curl_windows_clean:
#	$(MAKE) -fMAKEFILE_WINDOWS.MK -C$(dvr_rdk_PATH)/makerules curl_clean
#zlib_windows_clean:
#	$(MAKE) -fMAKEFILE_WINDOWS.MK -C$(dvr_rdk_PATH)/makerules zlib_clean
json_windows_clean:
	$(AMKE) -fMAKEFILE_WINDOWS.MK -C$(dvr_rdk_PATH)/makerules json_clean
#ffmpeg_windows_clean:
#	$(AMKE) -fMAKEFILE_WINDOWS.MK -C$(dvr_rdk_PATH)/makerules ffmpeg_clean
#lighttpd_windows_clean:
#	$(MAKE) -fMAKEFILE_WINDOWS.MK -C$(dvr_rdk_PATH)/makerules lighttpd_clean
#pcre_windows_clean:
#	$(MAKE) -fMAKEFILE_WINDOWS.MK -C$(dvr_rdk_PATH)/makerules pcre_clean
#mosquitto_windows_clean:
#	$(MAKE) -fMAKEFILE_WINDOWS.MK -C$(dvr_rdk_PATH)/makerules mosquitto_clean
openssl_windows_clean:
	$(MAKE) -fMAKEFILE_WINDOWS.MK -C$(dvr_rdk_PATH)/makerules openssl_clean

thirdlib_windows_release: json_windows_release openssl_windows_release 
thirdlib_windows_debug: json_windows_debug openssl_windows_debug
thirdlib_windows_clean:  json_windows_clean openssl_windows_clean 

# include $(dvr_rdk_PATH)/makerules/thirdlib.mk
# include $(dvr_rdk_PATH)/makerules/package.mk
