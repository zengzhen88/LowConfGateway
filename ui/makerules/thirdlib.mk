######################################################
#jgd215
curl_jgd215:
	$(MAKE) -fMAKEFILE_JGD215.MK -C$(dvr_rdk_PATH)/makerules curl
zlib_jgd215:
	$(MAKE) -fMAKEFILE_JGD215.MK -C$(dvr_rdk_PATH)/makerules zlib
json_jgd215:
	$(MAKE) -fMAKEFILE_JGD215.MK -C$(dvr_rdk_PATH)/makerules json
ffmpeg_jgd215:
	$(MAKE) -fMAKEFILE_JGD215.MK -C$(dvr_rdk_PATH)/makerules ffmpeg
lighttpd_jgd215:
	$(MAKE) -fMAKEFILE_JGD215.MK -C$(dvr_rdk_PATH)/makerules lighttpd
pcre_jgd215:
	$(MAKE) -fMAKEFILE_JGD215.MK -C$(dvr_rdk_PATH)/makerules pcre
mosquitto_jgd215:
	$(MAKE) -fMAKEFILE_JGD215.MK -C$(dvr_rdk_PATH)/makerules mosquitto
openssl_jgd215:
	$(MAKE) -fMAKEFILE_JGD215.MK -C$(dvr_rdk_PATH)/makerules openssl
cgic_jgd215:
	$(MAKE) -fMAKEFILE_JGD215.MK -C$(dvr_rdk_PATH)/makerules cgic
uv_jgd215:
	$(MAKE) -fMAKEFILE_JGD215.MK -C$(dvr_rdk_PATH)/makerules uv

curl_jgd215_clean:
	$(MAKE) -fMAKEFILE_JGD215.MK -C$(dvr_rdk_PATH)/makerules curl_clean
zlib_jgd215_clean:
	$(MAKE) -fMAKEFILE_JGD215.MK -C$(dvr_rdk_PATH)/makerules zlib_clean
json_jgd215_clean:
	$(MAKE) -fMAKEFILE_JGD215.MK -C$(dvr_rdk_PATH)/makerules json_clean
ffmpeg_jgd215_clean:
	$(MAKE) -fMAKEFILE_JGD215.MK -C$(dvr_rdk_PATH)/makerules ffmpeg_clean
lighttpd_jgd215_clean:
	$(MAKE) -fMAKEFILE_JGD215.MK -C$(dvr_rdk_PATH)/makerules lighttpd_clean
pcre_jgd215_clean:
	$(MAKE) -fMAKEFILE_JGD215.MK -C$(dvr_rdk_PATH)/makerules pcre_clean
mosquitto_jgd215_clean:
	$(MAKE) -fMAKEFILE_JGD215.MK -C$(dvr_rdk_PATH)/makerules mosquitto_clean
openssl_jgd215_clean:
	$(MAKE) -fMAKEFILE_JGD215.MK -C$(dvr_rdk_PATH)/makerules openssl_clean
cgic_jgd215_clean:
	$(MAKE) -fMAKEFILE_JGD215.MK -C$(dvr_rdk_PATH)/makerules cgic_clean
uv_jgd215_clean:
	$(MAKE) -fMAKEFILE_JGD215.MK -C$(dvr_rdk_PATH)/makerules uv_clean

# thirdlib_jgd215: zlib_jgd215 curl_jgd215 json_jgd215 ffmpeg_jgd215 pcre_jgd215 lighttpd_jgd215 uv_jgd215
thirdlib_jgd215: json_jgd215 ffmpeg_jgd215  openssl_jgd215 mosquitto_jgd215
thirdlib_jgd215_clean: json_jgd215_clean ffmpeg_jgd215_clean  openssl_jgd215_clean mosquitto_jgd215_clean

######################################################

######################################################
#ubuntu
curl_ubuntu:
	$(MAKE) -fMAKEFILE_UBUNTU.MK -C$(dvr_rdk_PATH)/makerules curl
zlib_ubuntu:
	$(MAKE) -fMAKEFILE_UBUNTU.MK -C$(dvr_rdk_PATH)/makerules zlib
json_ubuntu:
	$(MAKE) -fMAKEFILE_UBUNTU.MK -C$(dvr_rdk_PATH)/makerules json
ffmpeg_ubuntu:
	$(MAKE) -fMAKEFILE_UBUNTU.MK -C$(dvr_rdk_PATH)/makerules ffmpegs
lighttpd_ubuntu:
	$(MAKE) -fMAKEFILE_UBUNTU.MK -C$(dvr_rdk_PATH)/makerules lighttpd
pcre_ubuntu:
	$(MAKE) -fMAKEFILE_UBUNTU.MK -C$(dvr_rdk_PATH)/makerules pcre
mosquitto_ubuntu:
	$(MAKE) -fMAKEFILE_UBUNTU.MK -C$(dvr_rdk_PATH)/makerules mosquitto
lvgl_ubuntu:
	$(MAKE) -fMAKEFILE_UBUNTU.MK -C$(dvr_rdk_PATH)/makerules lvgl
openssl_ubuntu:
	$(MAKE) -fMAKEFILE_UBUNTU.MK -C$(dvr_rdk_PATH)/makerules openssls

curl_ubuntu_clean:
	$(MAKE) -fMAKEFILE_UBUNTU.MK -C$(dvr_rdk_PATH)/makerules curl_clean
zlib_ubuntu_clean:
	$(MAKE) -fMAKEFILE_UBUNTU.MK -C$(dvr_rdk_PATH)/makerules zlib_clean
json_ubuntu_clean:
	$(AMKE) -fMAKEFILE_UBUNTU.MK -C$(dvr_rdk_PATH)/makerules json_clean
ffmpeg_ubuntu_clean:
	$(AMKE) -fMAKEFILE_UBUNTU.MK -C$(dvr_rdk_PATH)/makerules ffmpeg_clean
lighttpd_ubuntu_clean:
	$(MAKE) -fMAKEFILE_UBUNTU.MK -C$(dvr_rdk_PATH)/makerules lighttpd_clean
pcre_ubuntu_clean:
	$(MAKE) -fMAKEFILE_UBUNTU.MK -C$(dvr_rdk_PATH)/makerules pcre_clean
mosquitto_ubuntu_clean:
	$(MAKE) -fMAKEFILE_UBUNTU.MK -C$(dvr_rdk_PATH)/makerules mosquitto_clean
lvgl_ubuntu_clean:
	$(MAKE) -fMAKEFILE_UBUNTU.MK -C$(dvr_rdk_PATH)/makerules lvgl_clean
openssl_ubuntu_clean:
	$(MAKE) -fMAKEFILE_UBUNTU.MK -C$(dvr_rdk_PATH)/makerules openssl_clean

# thirdlib_ubuntu:  json_ubuntu ffmpeg_ubuntu 
thirdlib_ubuntu: lvgl_ubuntu
thirdlib_ubuntu_clean:  lvgl_ubuntu_clean

######################################################

