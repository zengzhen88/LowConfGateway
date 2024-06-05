# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# compile ASM with /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc
# compile C with /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc
ASM_FLAGS = -mlongcalls    -ffunction-sections -fdata-sections -Wall -Werror=all -Wno-error=unused-function -Wno-error=unused-variable -Wno-error=unused-but-set-variable -Wno-error=deprecated-declarations -Wextra -Wno-unused-parameter -Wno-sign-compare -Wno-enum-conversion -gdwarf-4 -ggdb -Og -fno-shrink-wrap -fmacro-prefix-map=/home/zeng/share/esp/esp-idf/gateway=. -fmacro-prefix-map=/home/zeng/share/esp/esp-idf/esp-idf=/IDF -fstrict-volatile-bitfields -fno-jump-tables -fno-tree-switch-conversion

ASM_DEFINES = -DESP_PLATFORM -DIDF_VER=\"v5.4-dev-790-gc7bbfaee25\" -DMBEDTLS_CONFIG_FILE=\"mbedtls/esp_config.h\" -DSOC_MMU_PAGE_SIZE=CONFIG_MMU_PAGE_SIZE -DSOC_XTAL_FREQ_MHZ=CONFIG_XTAL_FREQ -D_GLIBCXX_HAVE_POSIX_SEMAPHORE -D_GLIBCXX_USE_POSIX_SEMAPHORE -D_GNU_SOURCE -D_POSIX_READER_WRITER_LOCKS

ASM_INCLUDES = -I/home/zeng/share/esp/esp-idf/gateway/build/config -I/home/zeng/share/esp/esp-idf/esp-idf/components/mbedtls/port/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/mbedtls/mbedtls/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/mbedtls/mbedtls/library -I/home/zeng/share/esp/esp-idf/esp-idf/components/mbedtls/esp_crt_bundle/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/newlib/platform_include -I/home/zeng/share/esp/esp-idf/esp-idf/components/freertos/config/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/freertos/config/include/freertos -I/home/zeng/share/esp/esp-idf/esp-idf/components/freertos/config/xtensa/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/freertos/FreeRTOS-Kernel/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/freertos/FreeRTOS-Kernel/portable/xtensa/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/freertos/FreeRTOS-Kernel/portable/xtensa/include/freertos -I/home/zeng/share/esp/esp-idf/esp-idf/components/freertos/esp_additions/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/esp_hw_support/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/esp_hw_support/include/soc -I/home/zeng/share/esp/esp-idf/esp-idf/components/esp_hw_support/include/soc/esp32 -I/home/zeng/share/esp/esp-idf/esp-idf/components/esp_hw_support/dma/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/esp_hw_support/ldo/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/esp_hw_support/port/esp32/. -I/home/zeng/share/esp/esp-idf/esp-idf/components/esp_hw_support/port/esp32/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/heap/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/log/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/soc/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/soc/esp32 -I/home/zeng/share/esp/esp-idf/esp-idf/components/soc/esp32/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/hal/platform_port/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/hal/esp32/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/hal/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/esp_rom/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/esp_rom/include/esp32 -I/home/zeng/share/esp/esp-idf/esp-idf/components/esp_rom/esp32 -I/home/zeng/share/esp/esp-idf/esp-idf/components/esp_common/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/esp_system/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/esp_system/port/soc -I/home/zeng/share/esp/esp-idf/esp-idf/components/esp_system/port/include/private -I/home/zeng/share/esp/esp-idf/esp-idf/components/xtensa/esp32/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/xtensa/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/xtensa/deprecated_include -I/home/zeng/share/esp/esp-idf/esp-idf/components/lwip/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/lwip/include/apps -I/home/zeng/share/esp/esp-idf/esp-idf/components/lwip/include/apps/sntp -I/home/zeng/share/esp/esp-idf/esp-idf/components/lwip/lwip/src/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/lwip/port/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/lwip/port/freertos/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/lwip/port/esp32xx/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/lwip/port/esp32xx/include/arch -I/home/zeng/share/esp/esp-idf/esp-idf/components/lwip/port/esp32xx/include/sys -I/home/zeng/share/esp/esp-idf/esp-idf/components/esp_pm/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/mbedtls/mbedtls/3rdparty/everest/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/mbedtls/mbedtls/3rdparty/p256-m -I/home/zeng/share/esp/esp-idf/esp-idf/components/mbedtls/mbedtls/3rdparty/p256-m/p256-m 

C_FLAGS = -mlongcalls -Wno-frame-address  -fno-builtin-memcpy -fno-builtin-memset -fno-builtin-bzero -fno-builtin-stpcpy -fno-builtin-strncpy   -ffunction-sections -fdata-sections -Wall -Werror=all -Wno-error=unused-function -Wno-error=unused-variable -Wno-error=unused-but-set-variable -Wno-error=deprecated-declarations -Wextra -Wno-unused-parameter -Wno-sign-compare -Wno-enum-conversion -gdwarf-4 -ggdb -Og -fno-shrink-wrap -fmacro-prefix-map=/home/zeng/share/esp/esp-idf/gateway=. -fmacro-prefix-map=/home/zeng/share/esp/esp-idf/esp-idf=/IDF -fstrict-volatile-bitfields -fno-jump-tables -fno-tree-switch-conversion -std=gnu17 -Wno-old-style-declaration

C_DEFINES = -DESP_PLATFORM -DIDF_VER=\"v5.4-dev-790-gc7bbfaee25\" -DMBEDTLS_CONFIG_FILE=\"mbedtls/esp_config.h\" -DSOC_MMU_PAGE_SIZE=CONFIG_MMU_PAGE_SIZE -DSOC_XTAL_FREQ_MHZ=CONFIG_XTAL_FREQ -D_GLIBCXX_HAVE_POSIX_SEMAPHORE -D_GLIBCXX_USE_POSIX_SEMAPHORE -D_GNU_SOURCE -D_POSIX_READER_WRITER_LOCKS

C_INCLUDES = -I/home/zeng/share/esp/esp-idf/gateway/build/config -I/home/zeng/share/esp/esp-idf/esp-idf/components/mbedtls/port/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/mbedtls/mbedtls/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/mbedtls/mbedtls/library -I/home/zeng/share/esp/esp-idf/esp-idf/components/mbedtls/esp_crt_bundle/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/newlib/platform_include -I/home/zeng/share/esp/esp-idf/esp-idf/components/freertos/config/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/freertos/config/include/freertos -I/home/zeng/share/esp/esp-idf/esp-idf/components/freertos/config/xtensa/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/freertos/FreeRTOS-Kernel/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/freertos/FreeRTOS-Kernel/portable/xtensa/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/freertos/FreeRTOS-Kernel/portable/xtensa/include/freertos -I/home/zeng/share/esp/esp-idf/esp-idf/components/freertos/esp_additions/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/esp_hw_support/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/esp_hw_support/include/soc -I/home/zeng/share/esp/esp-idf/esp-idf/components/esp_hw_support/include/soc/esp32 -I/home/zeng/share/esp/esp-idf/esp-idf/components/esp_hw_support/dma/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/esp_hw_support/ldo/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/esp_hw_support/port/esp32/. -I/home/zeng/share/esp/esp-idf/esp-idf/components/esp_hw_support/port/esp32/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/heap/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/log/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/soc/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/soc/esp32 -I/home/zeng/share/esp/esp-idf/esp-idf/components/soc/esp32/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/hal/platform_port/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/hal/esp32/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/hal/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/esp_rom/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/esp_rom/include/esp32 -I/home/zeng/share/esp/esp-idf/esp-idf/components/esp_rom/esp32 -I/home/zeng/share/esp/esp-idf/esp-idf/components/esp_common/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/esp_system/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/esp_system/port/soc -I/home/zeng/share/esp/esp-idf/esp-idf/components/esp_system/port/include/private -I/home/zeng/share/esp/esp-idf/esp-idf/components/xtensa/esp32/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/xtensa/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/xtensa/deprecated_include -I/home/zeng/share/esp/esp-idf/esp-idf/components/lwip/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/lwip/include/apps -I/home/zeng/share/esp/esp-idf/esp-idf/components/lwip/include/apps/sntp -I/home/zeng/share/esp/esp-idf/esp-idf/components/lwip/lwip/src/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/lwip/port/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/lwip/port/freertos/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/lwip/port/esp32xx/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/lwip/port/esp32xx/include/arch -I/home/zeng/share/esp/esp-idf/esp-idf/components/lwip/port/esp32xx/include/sys -I/home/zeng/share/esp/esp-idf/esp-idf/components/esp_pm/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/mbedtls/mbedtls/3rdparty/everest/include -I/home/zeng/share/esp/esp-idf/esp-idf/components/mbedtls/mbedtls/3rdparty/p256-m -I/home/zeng/share/esp/esp-idf/esp-idf/components/mbedtls/mbedtls/3rdparty/p256-m/p256-m 

