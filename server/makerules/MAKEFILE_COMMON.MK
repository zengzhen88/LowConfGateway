
####################################

#####################################
###AudApiSDK
JGDSDK_app:
	$(MAKE) -fMakefile -C$(AUD_API_SDK_PATH) app
JGDSDK_app_clean:
	$(MAKE) -fMakefile -C$(AUD_API_SDK_PATH) app_clean

JGDSDK_lib:
	$(MAKE) -fMakefile -C$(AUD_API_SDK_PATH) lib
JGDSDK_lib_clean:
	$(MAKE) -fMakefile -C$(AUD_API_SDK_PATH) lib_clean

#####################################

######
######
#common_third_lib
#curl
curl:curl_clean
	tar -xvf $(thrpartylib_PATH)/package/$(LIBCURL_VERSION)$(TAR_SUFF_BZ) -C $(thrpartylib_PATH)/
	cd $(thrpartylib_PATH)/$(LIBCURL_VERSION)/
	mkdir -p $(thrpartylib_PATH)/common/$(LIBCURL_VERSION)/$(PLATFORM)
	mkdir -p $(pre_BUILD_PATH)/$(PLATFORM)/lib
	mkdir -p $(pre_BUILD_PATH)/$(PLATFORM)/inc
	export CC=$(CODEGEN_PREFIX)gcc;                     							\
	cd $(thrpartylib_PATH)/$(LIBCURL_VERSION)/;            							\
	./configure --prefix=$(thrpartylib_PATH)/common/$(LIBCURL_VERSION)/$(PLATFORM)    	\
         --host=$(CODEGEN_HOST) --enable-static --without-ssl --without-zlib
	make -C $(thrpartylib_PATH)/$(LIBCURL_VERSION)/
	make -C $(thrpartylib_PATH)/$(LIBCURL_VERSION)/ install
	cp -rfd $(thrpartylib_PATH)/common/$(LIBCURL_VERSION)/$(PLATFORM)/lib/lib*.a $(pre_BUILD_PATH)/$(PLATFORM)/lib
	cp -rfd $(thrpartylib_PATH)/common/$(LIBCURL_VERSION)/$(PLATFORM)/include/* $(pre_BUILD_PATH)/$(PLATFORM)/inc

curl_clean:
	rm -rf $(thrpartylib_PATH)/$(LIBCURL_VERSION)

#zlib
zlib:zlib_clean
	tar -xvf $(thrpartylib_PATH)/package/$(LIBZLIB_VERSION)$(TAR_SUFF_GZ) -C $(thrpartylib_PATH)/
	cd $(thrpartylib_PATH)/$(LIBZLIB_VERSION)/
	mkdir -p $(thrpartylib_PATH)/common/$(LIBZLIB_VERSION)/$(PLATFORM)
	mkdir -p $(pre_BUILD_PATH)/$(PLATFORM)/lib
	mkdir -p $(pre_BUILD_PATH)/$(PLATFORM)/inc
	export CC=$(CODEGEN_PREFIX)gcc;                     							\
	cd $(thrpartylib_PATH)/$(LIBZLIB_VERSION)/;            							\
	./configure --prefix=$(thrpartylib_PATH)/common/$(LIBZLIB_VERSION)/$(PLATFORM)    	\
			--static
	make -C $(thrpartylib_PATH)/$(LIBZLIB_VERSION)/
	make -C $(thrpartylib_PATH)/$(LIBZLIB_VERSION)/ install
	cp -rfd $(thrpartylib_PATH)/common/$(LIBZLIB_VERSION)/$(PLATFORM)/lib/lib*.a $(pre_BUILD_PATH)/$(PLATFORM)/lib
	cp -rfd $(thrpartylib_PATH)/common/$(LIBZLIB_VERSION)/$(PLATFORM)/include/* $(pre_BUILD_PATH)/$(PLATFORM)/inc

zlib_clean:
	rm -rf $(thrpartylib_PATH)/$(LIBZLIB_VERSION)

#json
json:json_clean
	tar -xvf $(thrpartylib_PATH)/package/$(CJSONLIB_VERSION)$(TAR_SUFF_GZ) -C $(thrpartylib_PATH)/
	cd $(thrpartylib_PATH)/$(CJSONLIB_VERSION)/
	mkdir -p $(thrpartylib_PATH)/common/$(CJSONLIB_VERSION)/$(PLATFORM)
	mkdir -p $(pre_BUILD_PATH)/$(PLATFORM)/lib
	mkdir -p $(pre_BUILD_PATH)/$(PLATFORM)/inc
	export CC=$(CODEGEN_PREFIX)gcc;                     							\
	cd $(thrpartylib_PATH)/$(CJSONLIB_VERSION)/;            							\
	make -C $(thrpartylib_PATH)/$(CJSONLIB_VERSION)/
	make -C $(thrpartylib_PATH)/$(CJSONLIB_VERSION)/ DESTDIR=$(thrpartylib_PATH)/common/$(CJSONLIB_VERSION)/$(PLATFORM)/ install
	cp -rfd $(thrpartylib_PATH)/common/$(CJSONLIB_VERSION)/$(PLATFORM)/usr/local/lib/lib*.a $(pre_BUILD_PATH)/$(PLATFORM)/lib
	cp -rfd $(thrpartylib_PATH)/common/$(CJSONLIB_VERSION)/$(PLATFORM)/usr/local/include/* $(pre_BUILD_PATH)/$(PLATFORM)/inc

json_clean:
	rm -rf $(thrpartylib_PATH)/$(CJSONLIB_VERSION)

ffmpegs:ffmpeg_clean
	tar -xvf $(thrpartylib_PATH)/package/$(FFMPEGLIB_VERSION)$(TAR_SUFF_XZ) -C $(thrpartylib_PATH)/
	cd $(thrpartylib_PATH)/$(FFMPEGLIB_VERSION)/
	mkdir -p $(thrpartylib_PATH)/common/$(FFMPEGLIB_VERSION)/$(PLATFORM)
	mkdir -p $(pre_BUILD_PATH)/$(PLATFORM)/lib
	mkdir -p $(pre_BUILD_PATH)/$(PLATFORM)/inc
	export CC=$(CODEGEN_PREFIX)gcc;                     							\
	cd $(thrpartylib_PATH)/$(FFMPEGLIB_VERSION)/;            							\
	./configure --prefix=$(thrpartylib_PATH)/common/$(FFMPEGLIB_VERSION)/$(PLATFORM)  \
		--disable-ffmpeg 	\
		--disable-ffplay 	\
		--disable-ffprobe	\
		--disable-doc		\
		--disable-htmlpages	\
		--disable-avdevice	\
		--disable-avcodec	\
		--disable-avformat	\
		--disable-avcodec	\
		--disable-swresample\
		--disable-swscale	\
		--disable-postproc	\
		--disable-avfilter	\
		--disable-pthreads	\
		--disable-w32threads\
		--disable-os2threads\
		--disable-network	\
		--disable-dct		\
		--disable-dwt		\
		--disable-error-resilience	\
		--disable-lsp		\
		--disable-mdct 		\
		--disable-rdft		\
		--disable-fft 		\
		--disable-faan		\
		--disable-pixelutils \
		--disable-x86asm \
		--enable-small
	make -C $(thrpartylib_PATH)/$(FFMPEGLIB_VERSION)/
	make -C $(thrpartylib_PATH)/$(FFMPEGLIB_VERSION)/ install
	cp -rfd $(thrpartylib_PATH)/common/$(FFMPEGLIB_VERSION)/$(PLATFORM)/lib/lib*.a $(pre_BUILD_PATH)/$(PLATFORM)/lib
	cp -rfd $(thrpartylib_PATH)/common/$(FFMPEGLIB_VERSION)/$(PLATFORM)/include/* $(pre_BUILD_PATH)/$(PLATFORM)/inc

#ffmpeg
ffmpeg:ffmpeg_clean
	tar -xvf $(thrpartylib_PATH)/package/$(FFMPEGLIB_VERSION)$(TAR_SUFF_XZ) -C $(thrpartylib_PATH)/
	cd $(thrpartylib_PATH)/$(FFMPEGLIB_VERSION)/
	mkdir -p $(thrpartylib_PATH)/common/$(FFMPEGLIB_VERSION)/$(PLATFORM)
	mkdir -p $(pre_BUILD_PATH)/$(PLATFORM)/lib
	mkdir -p $(pre_BUILD_PATH)/$(PLATFORM)/inc
	export CC=$(CODEGEN_PREFIX)gcc;                     							\
	cd $(thrpartylib_PATH)/$(FFMPEGLIB_VERSION)/;            							\
	./configure --prefix=$(thrpartylib_PATH)/common/$(FFMPEGLIB_VERSION)/$(PLATFORM)  \
		--disable-ffmpeg 	\
		--disable-ffplay 	\
		--disable-ffprobe	\
		--disable-doc		\
		--disable-htmlpages	\
		--disable-avdevice	\
		--disable-avcodec	\
		--disable-avformat	\
		--disable-avcodec	\
		--disable-swresample\
		--disable-swscale	\
		--disable-postproc	\
		--disable-avfilter	\
		--disable-pthreads	\
		--disable-w32threads\
		--disable-os2threads\
		--disable-network	\
		--disable-dct		\
		--disable-dwt		\
		--disable-error-resilience	\
		--disable-lsp		\
		--disable-mdct 		\
		--disable-rdft		\
		--disable-fft 		\
		--disable-faan		\
		--disable-pixelutils \
		--enable-cross-compile \
		--cross-prefix=$(CODEGEN_PREFIX) \
		--enable-small \
		--arch=arm \
		--target-os=linux 
	make -C $(thrpartylib_PATH)/$(FFMPEGLIB_VERSION)/
	make -C $(thrpartylib_PATH)/$(FFMPEGLIB_VERSION)/ install
	cp -rfd $(thrpartylib_PATH)/common/$(FFMPEGLIB_VERSION)/$(PLATFORM)/lib/lib*.a $(pre_BUILD_PATH)/$(PLATFORM)/lib
	cp -rfd $(thrpartylib_PATH)/common/$(FFMPEGLIB_VERSION)/$(PLATFORM)/include/* $(pre_BUILD_PATH)/$(PLATFORM)/inc

ffmpeg_clean:
	rm -rf $(thrpartylib_PATH)/$(FFMPEGLIB_VERSION)

#lighttpd
lighttpd:lighttpd_clean
	tar -xvf $(thrpartylib_PATH)/package/$(LIGHTTPD_VERSION)$(TAR_SUFF_GZ) -C $(thrpartylib_PATH)/
	cd $(thrpartylib_PATH)/$(LIGHTTPD_VERSION)/
	mkdir -p $(thrpartylib_PATH)/common/$(LIGHTTPD_VERSION)/$(PLATFORM)
	mkdir -p $(pre_BUILD_PATH)/$(PLATFORM)/lib
	mkdir -p $(pre_BUILD_PATH)/$(PLATFORM)/inc
	export CC=$(CODEGEN_PREFIX)gcc;                     							\
	cd $(thrpartylib_PATH)/$(LIGHTTPD_VERSION)/;            							\
	./configure --prefix=$(thrpartylib_PATH)/common/$(LIGHTTPD_VERSION)/$(PLATFORM) --host=$(CODEGEN_HOST) --without-zlib \
	"CFLAGS=-I$(thrpartylib_PATH)/common/$(PCRE_VERSION)/$(PLATFORM)/include/" \
	"LDFLAGS=-L$(thrpartylib_PATH)/common/$(PCRE_VERSION)/$(PLATFORM)/lib/ -lpcre2-8" --enable-static --enable-shared=no
	make -C $(thrpartylib_PATH)/$(LIGHTTPD_VERSION)/
	make -C $(thrpartylib_PATH)/$(LIGHTTPD_VERSION)/ install
	# mkdir -p $(pre_BUILD_PATH)/$(PLATFORM)/lighttpd/cache
	# mkdir -p $(pre_BUILD_PATH)/$(PLATFORM)/lighttpd/cgi-bin
	# mkdir -p $(pre_BUILD_PATH)/$(PLATFORM)/lighttpd/config
	# mkdir -p $(pre_BUILD_PATH)/$(PLATFORM)/lighttpd/log
	# mkdir -p $(pre_BUILD_PATH)/$(PLATFORM)/lighttpd/sockets
	# mkdir -p $(pre_BUILD_PATH)/$(PLATFORM)/lighttpd/upload
	# mkdir -p $(pre_BUILD_PATH)/$(PLATFORM)/lighttpd/vhosts
	# mkdir -p $(pre_BUILD_PATH)/$(PLATFORM)/lighttpd/webpages
	# cp -rfd $(thrpartylib_PATH)/$(LIGHTTPD_VERSION)/doc/config/conf.d $(pre_BUILD_PATH)/$(PLATFORM)/lighttpd/config
	# cp -rfd $(thrpartylib_PATH)/$(LIGHTTPD_VERSION)/doc/config/lighttpd.conf $(pre_BUILD_PATH)/$(PLATFORM)/lighttpd/config
	# cp -rfd $(thrpartylib_PATH)/$(LIGHTTPD_VERSION)/doc/config/modules.conf $(pre_BUILD_PATH)/$(PLATFORM)/lighttpd/config
	cp -rfd $(thrpartylib_PATH)/common/$(LIGHTTPD_VERSION)/$(PLATFORM)/lib/*.so $(pre_BUILD_PATH)/$(PLATFORM)/lighttpd/lib
	# cp -rfd $(thrpartylib_PATH)/common/$(LIGHTTPD_VERSION)/$(PLATFORM)/include/* $(pre_BUILD_PATH)/$(PLATFORM)/inc
	cp -rfd $(thrpartylib_PATH)/common/$(LIGHTTPD_VERSION)/$(PLATFORM)/sbin/* $(pre_BUILD_PATH)/$(PLATFORM)/lighttpd/bin/

lighttpd_clean:
	rm -rf $(thrpartylib_PATH)/$(LIGHTTPD_VERSION)

#pcre
pcre:pcre_clean
	tar -xvf $(thrpartylib_PATH)/package/$(PCRE_VERSION)$(TAR_SUFF_GZ) -C $(thrpartylib_PATH)/
	cd $(thrpartylib_PATH)/$(PCRE_VERSION)/
	mkdir -p $(thrpartylib_PATH)/common/$(PCRE_VERSION)/$(PLATFORM)
	mkdir -p $(pre_BUILD_PATH)/$(PLATFORM)/lib
	mkdir -p $(pre_BUILD_PATH)/$(PLATFORM)/inc
	export CC=$(CODEGEN_PREFIX)gcc;                     							\
	cd $(thrpartylib_PATH)/$(PCRE_VERSION)/;            							\
	./configure --prefix=$(thrpartylib_PATH)/common/$(PCRE_VERSION)/$(PLATFORM) 	\
	--host=$(CODEGEN_HOST) --without-zlib --enable-shared=no
	make -C $(thrpartylib_PATH)/$(PCRE_VERSION)/
	make -C $(thrpartylib_PATH)/$(PCRE_VERSION)/ install
	cp -rfd $(thrpartylib_PATH)/common/$(PCRE_VERSION)/$(PLATFORM)/lib/lib*.a $(pre_BUILD_PATH)/$(PLATFORM)/lib
	cp -rfd $(thrpartylib_PATH)/common/$(PCRE_VERSION)/$(PLATFORM)/include/* $(pre_BUILD_PATH)/$(PLATFORM)/inc

pcre_clean:
	rm -rf $(thrpartylib_PATH)/$(PCRE_VERSION)

#uv
uv:uv_clean
	tar -xvf $(thrpartylib_PATH)/package/$(UV_VERSION)$(TAR_SUFF_BZ) -C $(thrpartylib_PATH)/
	cd $(thrpartylib_PATH)/$(UV_VERSION)/
	mkdir -p $(thrpartylib_PATH)/common/$(UV_VERSION)/$(PLATFORM)
	mkdir -p $(pre_BUILD_PATH)/$(PLATFORM)/lib
	mkdir -p $(pre_BUILD_PATH)/$(PLATFORM)/inc
	export CC=$(CODEGEN_PREFIX)gcc;                     							\
	cd $(thrpartylib_PATH)/$(UV_VERSION)/;            							\
	./configure --prefix=$(thrpartylib_PATH)/common/$(UV_VERSION)/$(PLATFORM) 	\
	--host=$(CODEGEN_HOST) --disable-shared --enable-static
	make -C $(thrpartylib_PATH)/$(UV_VERSION)/
	make -C $(thrpartylib_PATH)/$(UV_VERSION)/ install
	cp -rfd $(thrpartylib_PATH)/common/$(UV_VERSION)/$(PLATFORM)/lib/lib*.a $(pre_BUILD_PATH)/$(PLATFORM)/lib
	cp -rfd $(thrpartylib_PATH)/common/$(UV_VERSION)/$(PLATFORM)/include/* $(pre_BUILD_PATH)/$(PLATFORM)/inc

uv_clean:
	rm -rf $(thrpartylib_PATH)/$(UV_VERSION)

#cgic
cgic:cgic_clean
	tar -xvf $(thrpartylib_PATH)/package/$(MOSQUITTO_VERSION)$(TAR_SUFF_BZ) -C $(thrpartylib_PATH)/
	cd $(thrpartylib_PATH)/$(MOSQUITTO_VERSION)/
	mkdir -p $(thrpartylib_PATH)/common/$(MOSQUITTO_VERSION)/$(PLATFORM)
	mkdir -p $(pre_BUILD_PATH)/$(PLATFORM)/lib
	mkdir -p $(pre_BUILD_PATH)/$(PLATFORM)/inc
	export CC=$(CODEGEN_PREFIX)gcc;                     							\
	export AR=$(CODEGEN_PREFIX)ar; 												\
	cd $(thrpartylib_PATH)/$(MOSQUITTO_VERSION)/;            							\
	make -C $(thrpartylib_PATH)/$(MOSQUITTO_VERSION)/
	make -C $(thrpartylib_PATH)/$(MOSQUITTO_VERSION)/ DESTDIR=$(thrpartylib_PATH)/common/$(MOSQUITTO_VERSION)/$(PLATFORM)/ install
	cp -rfd $(thrpartylib_PATH)/common/$(MOSQUITTO_VERSION)/$(PLATFORM)/usr/local/lib/lib*.a $(pre_BUILD_PATH)/$(PLATFORM)/lib
	cp -rfd $(thrpartylib_PATH)/common/$(MOSQUITTO_VERSION)/$(PLATFORM)/usr/local/include/* $(pre_BUILD_PATH)/$(PLATFORM)/inc

cgic_clean:
	rm -rf $(thrpartylib_PATH)/$(MOSQUITTO_VERSION)

openssls:openssl_clean
	tar -xvf $(thrpartylib_PATH)/package/$(OPENSSL_VERSION)$(TAR_SUFF_GZ) -C $(thrpartylib_PATH)/
	cd $(thrpartylib_PATH)/$(OPENSSL_VERSION)/
	mkdir -p $(thrpartylib_PATH)/common/$(OPENSSL_VERSION)/$(PLATFORM)
	mkdir -p $(pre_BUILD_PATH)/$(PLATFORM)/lib
	mkdir -p $(pre_BUILD_PATH)/$(PLATFORM)/inc
	cd $(thrpartylib_PATH)/$(OPENSSL_VERSION)/;            							\
	./config --prefix=$(thrpartylib_PATH)/common/$(OPENSSL_VERSION)/$(PLATFORM) 	\
		-static 
	make CC=$(CODEGEN_PREFIX)gcc AR=$(CODEGEN_PREFIX)ar -C $(thrpartylib_PATH)/$(OPENSSL_VERSION)/
	make -C $(thrpartylib_PATH)/$(OPENSSL_VERSION)/  install
	cp -rfd $(thrpartylib_PATH)/common/$(OPENSSL_VERSION)/$(PLATFORM)/lib64/lib*.a $(pre_BUILD_PATH)/$(PLATFORM)/lib
	cp -rfd $(thrpartylib_PATH)/common/$(OPENSSL_VERSION)/$(PLATFORM)/include/* $(pre_BUILD_PATH)/$(PLATFORM)/inc
openssl:openssl_clean
	tar -xvf $(thrpartylib_PATH)/package/$(OPENSSL_VERSION)$(TAR_SUFF_GZ) -C $(thrpartylib_PATH)/
	cd $(thrpartylib_PATH)/$(OPENSSL_VERSION)/
	mkdir -p $(thrpartylib_PATH)/common/$(OPENSSL_VERSION)/$(PLATFORM)
	mkdir -p $(pre_BUILD_PATH)/$(PLATFORM)/lib
	mkdir -p $(pre_BUILD_PATH)/$(PLATFORM)/inc
	cd $(thrpartylib_PATH)/$(OPENSSL_VERSION)/;            							\
	./Configure --prefix=$(thrpartylib_PATH)/common/$(OPENSSL_VERSION)/$(PLATFORM) 	\
		--host=$(CODEGEN_HOST) --disable-shared --enable-static
	make CC=$(CODEGEN_PREFIX)gcc AR=$(CODEGEN_PREFIX)ar -C $(thrpartylib_PATH)/$(OPENSSL_VERSION)/
	make -C $(thrpartylib_PATH)/$(OPENSSL_VERSION)/ DESTDIR=$(thrpartylib_PATH)/common/$(OPENSSL_VERSION)/$(PLATFORM)/ install
	cp -rfd $(thrpartylib_PATH)/common/$(OPENSSL_VERSION)/$(PLATFORM)/usr/local/lib/lib*.a $(pre_BUILD_PATH)/$(PLATFORM)/lib
	cp -rfd $(thrpartylib_PATH)/common/$(OPENSSL_VERSION)/$(PLATFORM)/usr/local/include/* $(pre_BUILD_PATH)/$(PLATFORM)/inc

openssl_clean:
	rm -rf $(thrpartylib_PATH)/$(OPENSSL_VERSION)

mosquitto:mosquitto_clean
	tar -xvf $(thrpartylib_PATH)/package/$(MOSQUITTO_VERSION)$(TAR_SUFF_GZ) -C $(thrpartylib_PATH)/
	cd $(thrpartylib_PATH)/$(MOSQUITTO_VERSION)/
	mkdir -p $(thrpartylib_PATH)/common/$(MOSQUITTO_VERSION)/$(PLATFORM)
	mkdir -p $(pre_BUILD_PATH)/$(PLATFORM)/lib
	mkdir -p $(pre_BUILD_PATH)/$(PLATFORM)/inc
	cd $(thrpartylib_PATH)/$(MOSQUITTO_VERSION)/;            							\
	make CC=$(CODEGEN_PREFIX)gcc AR=$(CODEGEN_PREFIX)ar CFLAGS="-I$(pre_BUILD_PATH)/$(PLATFORM)/inc" LDFLAGS="-L$(pre_BUILD_PATH)/$(PLATFORM)/lib" -C $(thrpartylib_PATH)/$(MOSQUITTO_VERSION)/
	make -C $(thrpartylib_PATH)/$(MOSQUITTO_VERSION)/ DESTDIR=$(thrpartylib_PATH)/common/$(MOSQUITTO_VERSION)/$(PLATFORM)/ install
	cp -rfd $(thrpartylib_PATH)/common/$(MOSQUITTO_VERSION)/$(PLATFORM)/usr/local/lib/lib*.a $(pre_BUILD_PATH)/$(PLATFORM)/lib
	cp -rfd $(thrpartylib_PATH)/common/$(MOSQUITTO_VERSION)/$(PLATFORM)/usr/local/include/* $(pre_BUILD_PATH)/$(PLATFORM)/inc

mosquitto_clean:
	rm -rf $(thrpartylib_PATH)/$(MOSQUITTO_VERSION)

include $(dvr_rdk_PATH)/makerules/package.mk
