# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/zeng/share/esp/esp-idf/gateway

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/zeng/share/esp/esp-idf/gateway/build

# Include any dependencies generated for this target.
include esp-idf/driver/CMakeFiles/__idf_driver.dir/depend.make

# Include the progress variables for this target.
include esp-idf/driver/CMakeFiles/__idf_driver.dir/progress.make

# Include the compile flags for this target's objects.
include esp-idf/driver/CMakeFiles/__idf_driver.dir/flags.make

esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/adc_legacy.c.obj: esp-idf/driver/CMakeFiles/__idf_driver.dir/flags.make
esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/adc_legacy.c.obj: /home/zeng/share/esp/esp-idf/esp-idf/components/driver/deprecated/adc_legacy.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/zeng/share/esp/esp-idf/gateway/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/adc_legacy.c.obj"
	cd /home/zeng/share/esp/esp-idf/gateway/build/esp-idf/driver && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/__idf_driver.dir/deprecated/adc_legacy.c.obj   -c /home/zeng/share/esp/esp-idf/esp-idf/components/driver/deprecated/adc_legacy.c

esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/adc_legacy.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_driver.dir/deprecated/adc_legacy.c.i"
	cd /home/zeng/share/esp/esp-idf/gateway/build/esp-idf/driver && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/zeng/share/esp/esp-idf/esp-idf/components/driver/deprecated/adc_legacy.c > CMakeFiles/__idf_driver.dir/deprecated/adc_legacy.c.i

esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/adc_legacy.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_driver.dir/deprecated/adc_legacy.c.s"
	cd /home/zeng/share/esp/esp-idf/gateway/build/esp-idf/driver && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/zeng/share/esp/esp-idf/esp-idf/components/driver/deprecated/adc_legacy.c -o CMakeFiles/__idf_driver.dir/deprecated/adc_legacy.c.s

esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/adc_dma_legacy.c.obj: esp-idf/driver/CMakeFiles/__idf_driver.dir/flags.make
esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/adc_dma_legacy.c.obj: /home/zeng/share/esp/esp-idf/esp-idf/components/driver/deprecated/adc_dma_legacy.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/zeng/share/esp/esp-idf/gateway/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/adc_dma_legacy.c.obj"
	cd /home/zeng/share/esp/esp-idf/gateway/build/esp-idf/driver && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/__idf_driver.dir/deprecated/adc_dma_legacy.c.obj   -c /home/zeng/share/esp/esp-idf/esp-idf/components/driver/deprecated/adc_dma_legacy.c

esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/adc_dma_legacy.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_driver.dir/deprecated/adc_dma_legacy.c.i"
	cd /home/zeng/share/esp/esp-idf/gateway/build/esp-idf/driver && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/zeng/share/esp/esp-idf/esp-idf/components/driver/deprecated/adc_dma_legacy.c > CMakeFiles/__idf_driver.dir/deprecated/adc_dma_legacy.c.i

esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/adc_dma_legacy.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_driver.dir/deprecated/adc_dma_legacy.c.s"
	cd /home/zeng/share/esp/esp-idf/gateway/build/esp-idf/driver && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/zeng/share/esp/esp-idf/esp-idf/components/driver/deprecated/adc_dma_legacy.c -o CMakeFiles/__idf_driver.dir/deprecated/adc_dma_legacy.c.s

esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/dac_common_legacy.c.obj: esp-idf/driver/CMakeFiles/__idf_driver.dir/flags.make
esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/dac_common_legacy.c.obj: /home/zeng/share/esp/esp-idf/esp-idf/components/driver/deprecated/dac_common_legacy.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/zeng/share/esp/esp-idf/gateway/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/dac_common_legacy.c.obj"
	cd /home/zeng/share/esp/esp-idf/gateway/build/esp-idf/driver && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/__idf_driver.dir/deprecated/dac_common_legacy.c.obj   -c /home/zeng/share/esp/esp-idf/esp-idf/components/driver/deprecated/dac_common_legacy.c

esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/dac_common_legacy.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_driver.dir/deprecated/dac_common_legacy.c.i"
	cd /home/zeng/share/esp/esp-idf/gateway/build/esp-idf/driver && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/zeng/share/esp/esp-idf/esp-idf/components/driver/deprecated/dac_common_legacy.c > CMakeFiles/__idf_driver.dir/deprecated/dac_common_legacy.c.i

esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/dac_common_legacy.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_driver.dir/deprecated/dac_common_legacy.c.s"
	cd /home/zeng/share/esp/esp-idf/gateway/build/esp-idf/driver && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/zeng/share/esp/esp-idf/esp-idf/components/driver/deprecated/dac_common_legacy.c -o CMakeFiles/__idf_driver.dir/deprecated/dac_common_legacy.c.s

esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/esp32/dac_legacy.c.obj: esp-idf/driver/CMakeFiles/__idf_driver.dir/flags.make
esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/esp32/dac_legacy.c.obj: /home/zeng/share/esp/esp-idf/esp-idf/components/driver/deprecated/esp32/dac_legacy.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/zeng/share/esp/esp-idf/gateway/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/esp32/dac_legacy.c.obj"
	cd /home/zeng/share/esp/esp-idf/gateway/build/esp-idf/driver && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/__idf_driver.dir/deprecated/esp32/dac_legacy.c.obj   -c /home/zeng/share/esp/esp-idf/esp-idf/components/driver/deprecated/esp32/dac_legacy.c

esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/esp32/dac_legacy.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_driver.dir/deprecated/esp32/dac_legacy.c.i"
	cd /home/zeng/share/esp/esp-idf/gateway/build/esp-idf/driver && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/zeng/share/esp/esp-idf/esp-idf/components/driver/deprecated/esp32/dac_legacy.c > CMakeFiles/__idf_driver.dir/deprecated/esp32/dac_legacy.c.i

esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/esp32/dac_legacy.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_driver.dir/deprecated/esp32/dac_legacy.c.s"
	cd /home/zeng/share/esp/esp-idf/gateway/build/esp-idf/driver && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/zeng/share/esp/esp-idf/esp-idf/components/driver/deprecated/esp32/dac_legacy.c -o CMakeFiles/__idf_driver.dir/deprecated/esp32/dac_legacy.c.s

esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/timer_legacy.c.obj: esp-idf/driver/CMakeFiles/__idf_driver.dir/flags.make
esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/timer_legacy.c.obj: /home/zeng/share/esp/esp-idf/esp-idf/components/driver/deprecated/timer_legacy.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/zeng/share/esp/esp-idf/gateway/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building C object esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/timer_legacy.c.obj"
	cd /home/zeng/share/esp/esp-idf/gateway/build/esp-idf/driver && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/__idf_driver.dir/deprecated/timer_legacy.c.obj   -c /home/zeng/share/esp/esp-idf/esp-idf/components/driver/deprecated/timer_legacy.c

esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/timer_legacy.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_driver.dir/deprecated/timer_legacy.c.i"
	cd /home/zeng/share/esp/esp-idf/gateway/build/esp-idf/driver && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/zeng/share/esp/esp-idf/esp-idf/components/driver/deprecated/timer_legacy.c > CMakeFiles/__idf_driver.dir/deprecated/timer_legacy.c.i

esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/timer_legacy.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_driver.dir/deprecated/timer_legacy.c.s"
	cd /home/zeng/share/esp/esp-idf/gateway/build/esp-idf/driver && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/zeng/share/esp/esp-idf/esp-idf/components/driver/deprecated/timer_legacy.c -o CMakeFiles/__idf_driver.dir/deprecated/timer_legacy.c.s

esp-idf/driver/CMakeFiles/__idf_driver.dir/i2c/i2c.c.obj: esp-idf/driver/CMakeFiles/__idf_driver.dir/flags.make
esp-idf/driver/CMakeFiles/__idf_driver.dir/i2c/i2c.c.obj: /home/zeng/share/esp/esp-idf/esp-idf/components/driver/i2c/i2c.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/zeng/share/esp/esp-idf/gateway/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building C object esp-idf/driver/CMakeFiles/__idf_driver.dir/i2c/i2c.c.obj"
	cd /home/zeng/share/esp/esp-idf/gateway/build/esp-idf/driver && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/__idf_driver.dir/i2c/i2c.c.obj   -c /home/zeng/share/esp/esp-idf/esp-idf/components/driver/i2c/i2c.c

esp-idf/driver/CMakeFiles/__idf_driver.dir/i2c/i2c.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_driver.dir/i2c/i2c.c.i"
	cd /home/zeng/share/esp/esp-idf/gateway/build/esp-idf/driver && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/zeng/share/esp/esp-idf/esp-idf/components/driver/i2c/i2c.c > CMakeFiles/__idf_driver.dir/i2c/i2c.c.i

esp-idf/driver/CMakeFiles/__idf_driver.dir/i2c/i2c.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_driver.dir/i2c/i2c.c.s"
	cd /home/zeng/share/esp/esp-idf/gateway/build/esp-idf/driver && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/zeng/share/esp/esp-idf/esp-idf/components/driver/i2c/i2c.c -o CMakeFiles/__idf_driver.dir/i2c/i2c.c.s

esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/i2s_legacy.c.obj: esp-idf/driver/CMakeFiles/__idf_driver.dir/flags.make
esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/i2s_legacy.c.obj: /home/zeng/share/esp/esp-idf/esp-idf/components/driver/deprecated/i2s_legacy.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/zeng/share/esp/esp-idf/gateway/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building C object esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/i2s_legacy.c.obj"
	cd /home/zeng/share/esp/esp-idf/gateway/build/esp-idf/driver && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/__idf_driver.dir/deprecated/i2s_legacy.c.obj   -c /home/zeng/share/esp/esp-idf/esp-idf/components/driver/deprecated/i2s_legacy.c

esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/i2s_legacy.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_driver.dir/deprecated/i2s_legacy.c.i"
	cd /home/zeng/share/esp/esp-idf/gateway/build/esp-idf/driver && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/zeng/share/esp/esp-idf/esp-idf/components/driver/deprecated/i2s_legacy.c > CMakeFiles/__idf_driver.dir/deprecated/i2s_legacy.c.i

esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/i2s_legacy.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_driver.dir/deprecated/i2s_legacy.c.s"
	cd /home/zeng/share/esp/esp-idf/gateway/build/esp-idf/driver && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/zeng/share/esp/esp-idf/esp-idf/components/driver/deprecated/i2s_legacy.c -o CMakeFiles/__idf_driver.dir/deprecated/i2s_legacy.c.s

esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/mcpwm_legacy.c.obj: esp-idf/driver/CMakeFiles/__idf_driver.dir/flags.make
esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/mcpwm_legacy.c.obj: /home/zeng/share/esp/esp-idf/esp-idf/components/driver/deprecated/mcpwm_legacy.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/zeng/share/esp/esp-idf/gateway/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building C object esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/mcpwm_legacy.c.obj"
	cd /home/zeng/share/esp/esp-idf/gateway/build/esp-idf/driver && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/__idf_driver.dir/deprecated/mcpwm_legacy.c.obj   -c /home/zeng/share/esp/esp-idf/esp-idf/components/driver/deprecated/mcpwm_legacy.c

esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/mcpwm_legacy.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_driver.dir/deprecated/mcpwm_legacy.c.i"
	cd /home/zeng/share/esp/esp-idf/gateway/build/esp-idf/driver && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/zeng/share/esp/esp-idf/esp-idf/components/driver/deprecated/mcpwm_legacy.c > CMakeFiles/__idf_driver.dir/deprecated/mcpwm_legacy.c.i

esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/mcpwm_legacy.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_driver.dir/deprecated/mcpwm_legacy.c.s"
	cd /home/zeng/share/esp/esp-idf/gateway/build/esp-idf/driver && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/zeng/share/esp/esp-idf/esp-idf/components/driver/deprecated/mcpwm_legacy.c -o CMakeFiles/__idf_driver.dir/deprecated/mcpwm_legacy.c.s

esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/pcnt_legacy.c.obj: esp-idf/driver/CMakeFiles/__idf_driver.dir/flags.make
esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/pcnt_legacy.c.obj: /home/zeng/share/esp/esp-idf/esp-idf/components/driver/deprecated/pcnt_legacy.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/zeng/share/esp/esp-idf/gateway/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building C object esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/pcnt_legacy.c.obj"
	cd /home/zeng/share/esp/esp-idf/gateway/build/esp-idf/driver && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/__idf_driver.dir/deprecated/pcnt_legacy.c.obj   -c /home/zeng/share/esp/esp-idf/esp-idf/components/driver/deprecated/pcnt_legacy.c

esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/pcnt_legacy.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_driver.dir/deprecated/pcnt_legacy.c.i"
	cd /home/zeng/share/esp/esp-idf/gateway/build/esp-idf/driver && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/zeng/share/esp/esp-idf/esp-idf/components/driver/deprecated/pcnt_legacy.c > CMakeFiles/__idf_driver.dir/deprecated/pcnt_legacy.c.i

esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/pcnt_legacy.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_driver.dir/deprecated/pcnt_legacy.c.s"
	cd /home/zeng/share/esp/esp-idf/gateway/build/esp-idf/driver && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/zeng/share/esp/esp-idf/esp-idf/components/driver/deprecated/pcnt_legacy.c -o CMakeFiles/__idf_driver.dir/deprecated/pcnt_legacy.c.s

esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/rmt_legacy.c.obj: esp-idf/driver/CMakeFiles/__idf_driver.dir/flags.make
esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/rmt_legacy.c.obj: /home/zeng/share/esp/esp-idf/esp-idf/components/driver/deprecated/rmt_legacy.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/zeng/share/esp/esp-idf/gateway/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building C object esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/rmt_legacy.c.obj"
	cd /home/zeng/share/esp/esp-idf/gateway/build/esp-idf/driver && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/__idf_driver.dir/deprecated/rmt_legacy.c.obj   -c /home/zeng/share/esp/esp-idf/esp-idf/components/driver/deprecated/rmt_legacy.c

esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/rmt_legacy.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_driver.dir/deprecated/rmt_legacy.c.i"
	cd /home/zeng/share/esp/esp-idf/gateway/build/esp-idf/driver && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/zeng/share/esp/esp-idf/esp-idf/components/driver/deprecated/rmt_legacy.c > CMakeFiles/__idf_driver.dir/deprecated/rmt_legacy.c.i

esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/rmt_legacy.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_driver.dir/deprecated/rmt_legacy.c.s"
	cd /home/zeng/share/esp/esp-idf/gateway/build/esp-idf/driver && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/zeng/share/esp/esp-idf/esp-idf/components/driver/deprecated/rmt_legacy.c -o CMakeFiles/__idf_driver.dir/deprecated/rmt_legacy.c.s

esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/sigma_delta_legacy.c.obj: esp-idf/driver/CMakeFiles/__idf_driver.dir/flags.make
esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/sigma_delta_legacy.c.obj: /home/zeng/share/esp/esp-idf/esp-idf/components/driver/deprecated/sigma_delta_legacy.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/zeng/share/esp/esp-idf/gateway/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Building C object esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/sigma_delta_legacy.c.obj"
	cd /home/zeng/share/esp/esp-idf/gateway/build/esp-idf/driver && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/__idf_driver.dir/deprecated/sigma_delta_legacy.c.obj   -c /home/zeng/share/esp/esp-idf/esp-idf/components/driver/deprecated/sigma_delta_legacy.c

esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/sigma_delta_legacy.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_driver.dir/deprecated/sigma_delta_legacy.c.i"
	cd /home/zeng/share/esp/esp-idf/gateway/build/esp-idf/driver && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/zeng/share/esp/esp-idf/esp-idf/components/driver/deprecated/sigma_delta_legacy.c > CMakeFiles/__idf_driver.dir/deprecated/sigma_delta_legacy.c.i

esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/sigma_delta_legacy.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_driver.dir/deprecated/sigma_delta_legacy.c.s"
	cd /home/zeng/share/esp/esp-idf/gateway/build/esp-idf/driver && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/zeng/share/esp/esp-idf/esp-idf/components/driver/deprecated/sigma_delta_legacy.c -o CMakeFiles/__idf_driver.dir/deprecated/sigma_delta_legacy.c.s

esp-idf/driver/CMakeFiles/__idf_driver.dir/touch_sensor/touch_sensor_common.c.obj: esp-idf/driver/CMakeFiles/__idf_driver.dir/flags.make
esp-idf/driver/CMakeFiles/__idf_driver.dir/touch_sensor/touch_sensor_common.c.obj: /home/zeng/share/esp/esp-idf/esp-idf/components/driver/touch_sensor/touch_sensor_common.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/zeng/share/esp/esp-idf/gateway/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_12) "Building C object esp-idf/driver/CMakeFiles/__idf_driver.dir/touch_sensor/touch_sensor_common.c.obj"
	cd /home/zeng/share/esp/esp-idf/gateway/build/esp-idf/driver && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/__idf_driver.dir/touch_sensor/touch_sensor_common.c.obj   -c /home/zeng/share/esp/esp-idf/esp-idf/components/driver/touch_sensor/touch_sensor_common.c

esp-idf/driver/CMakeFiles/__idf_driver.dir/touch_sensor/touch_sensor_common.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_driver.dir/touch_sensor/touch_sensor_common.c.i"
	cd /home/zeng/share/esp/esp-idf/gateway/build/esp-idf/driver && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/zeng/share/esp/esp-idf/esp-idf/components/driver/touch_sensor/touch_sensor_common.c > CMakeFiles/__idf_driver.dir/touch_sensor/touch_sensor_common.c.i

esp-idf/driver/CMakeFiles/__idf_driver.dir/touch_sensor/touch_sensor_common.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_driver.dir/touch_sensor/touch_sensor_common.c.s"
	cd /home/zeng/share/esp/esp-idf/gateway/build/esp-idf/driver && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/zeng/share/esp/esp-idf/esp-idf/components/driver/touch_sensor/touch_sensor_common.c -o CMakeFiles/__idf_driver.dir/touch_sensor/touch_sensor_common.c.s

esp-idf/driver/CMakeFiles/__idf_driver.dir/touch_sensor/esp32/touch_sensor.c.obj: esp-idf/driver/CMakeFiles/__idf_driver.dir/flags.make
esp-idf/driver/CMakeFiles/__idf_driver.dir/touch_sensor/esp32/touch_sensor.c.obj: /home/zeng/share/esp/esp-idf/esp-idf/components/driver/touch_sensor/esp32/touch_sensor.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/zeng/share/esp/esp-idf/gateway/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_13) "Building C object esp-idf/driver/CMakeFiles/__idf_driver.dir/touch_sensor/esp32/touch_sensor.c.obj"
	cd /home/zeng/share/esp/esp-idf/gateway/build/esp-idf/driver && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/__idf_driver.dir/touch_sensor/esp32/touch_sensor.c.obj   -c /home/zeng/share/esp/esp-idf/esp-idf/components/driver/touch_sensor/esp32/touch_sensor.c

esp-idf/driver/CMakeFiles/__idf_driver.dir/touch_sensor/esp32/touch_sensor.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_driver.dir/touch_sensor/esp32/touch_sensor.c.i"
	cd /home/zeng/share/esp/esp-idf/gateway/build/esp-idf/driver && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/zeng/share/esp/esp-idf/esp-idf/components/driver/touch_sensor/esp32/touch_sensor.c > CMakeFiles/__idf_driver.dir/touch_sensor/esp32/touch_sensor.c.i

esp-idf/driver/CMakeFiles/__idf_driver.dir/touch_sensor/esp32/touch_sensor.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_driver.dir/touch_sensor/esp32/touch_sensor.c.s"
	cd /home/zeng/share/esp/esp-idf/gateway/build/esp-idf/driver && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/zeng/share/esp/esp-idf/esp-idf/components/driver/touch_sensor/esp32/touch_sensor.c -o CMakeFiles/__idf_driver.dir/touch_sensor/esp32/touch_sensor.c.s

esp-idf/driver/CMakeFiles/__idf_driver.dir/twai/twai.c.obj: esp-idf/driver/CMakeFiles/__idf_driver.dir/flags.make
esp-idf/driver/CMakeFiles/__idf_driver.dir/twai/twai.c.obj: /home/zeng/share/esp/esp-idf/esp-idf/components/driver/twai/twai.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/zeng/share/esp/esp-idf/gateway/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_14) "Building C object esp-idf/driver/CMakeFiles/__idf_driver.dir/twai/twai.c.obj"
	cd /home/zeng/share/esp/esp-idf/gateway/build/esp-idf/driver && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/__idf_driver.dir/twai/twai.c.obj   -c /home/zeng/share/esp/esp-idf/esp-idf/components/driver/twai/twai.c

esp-idf/driver/CMakeFiles/__idf_driver.dir/twai/twai.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_driver.dir/twai/twai.c.i"
	cd /home/zeng/share/esp/esp-idf/gateway/build/esp-idf/driver && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/zeng/share/esp/esp-idf/esp-idf/components/driver/twai/twai.c > CMakeFiles/__idf_driver.dir/twai/twai.c.i

esp-idf/driver/CMakeFiles/__idf_driver.dir/twai/twai.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_driver.dir/twai/twai.c.s"
	cd /home/zeng/share/esp/esp-idf/gateway/build/esp-idf/driver && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/zeng/share/esp/esp-idf/esp-idf/components/driver/twai/twai.c -o CMakeFiles/__idf_driver.dir/twai/twai.c.s

esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/adc_i2s_deprecated.c.obj: esp-idf/driver/CMakeFiles/__idf_driver.dir/flags.make
esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/adc_i2s_deprecated.c.obj: /home/zeng/share/esp/esp-idf/esp-idf/components/driver/deprecated/adc_i2s_deprecated.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/zeng/share/esp/esp-idf/gateway/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_15) "Building C object esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/adc_i2s_deprecated.c.obj"
	cd /home/zeng/share/esp/esp-idf/gateway/build/esp-idf/driver && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/__idf_driver.dir/deprecated/adc_i2s_deprecated.c.obj   -c /home/zeng/share/esp/esp-idf/esp-idf/components/driver/deprecated/adc_i2s_deprecated.c

esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/adc_i2s_deprecated.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_driver.dir/deprecated/adc_i2s_deprecated.c.i"
	cd /home/zeng/share/esp/esp-idf/gateway/build/esp-idf/driver && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/zeng/share/esp/esp-idf/esp-idf/components/driver/deprecated/adc_i2s_deprecated.c > CMakeFiles/__idf_driver.dir/deprecated/adc_i2s_deprecated.c.i

esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/adc_i2s_deprecated.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_driver.dir/deprecated/adc_i2s_deprecated.c.s"
	cd /home/zeng/share/esp/esp-idf/gateway/build/esp-idf/driver && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/zeng/share/esp/esp-idf/esp-idf/components/driver/deprecated/adc_i2s_deprecated.c -o CMakeFiles/__idf_driver.dir/deprecated/adc_i2s_deprecated.c.s

# Object files for target __idf_driver
__idf_driver_OBJECTS = \
"CMakeFiles/__idf_driver.dir/deprecated/adc_legacy.c.obj" \
"CMakeFiles/__idf_driver.dir/deprecated/adc_dma_legacy.c.obj" \
"CMakeFiles/__idf_driver.dir/deprecated/dac_common_legacy.c.obj" \
"CMakeFiles/__idf_driver.dir/deprecated/esp32/dac_legacy.c.obj" \
"CMakeFiles/__idf_driver.dir/deprecated/timer_legacy.c.obj" \
"CMakeFiles/__idf_driver.dir/i2c/i2c.c.obj" \
"CMakeFiles/__idf_driver.dir/deprecated/i2s_legacy.c.obj" \
"CMakeFiles/__idf_driver.dir/deprecated/mcpwm_legacy.c.obj" \
"CMakeFiles/__idf_driver.dir/deprecated/pcnt_legacy.c.obj" \
"CMakeFiles/__idf_driver.dir/deprecated/rmt_legacy.c.obj" \
"CMakeFiles/__idf_driver.dir/deprecated/sigma_delta_legacy.c.obj" \
"CMakeFiles/__idf_driver.dir/touch_sensor/touch_sensor_common.c.obj" \
"CMakeFiles/__idf_driver.dir/touch_sensor/esp32/touch_sensor.c.obj" \
"CMakeFiles/__idf_driver.dir/twai/twai.c.obj" \
"CMakeFiles/__idf_driver.dir/deprecated/adc_i2s_deprecated.c.obj"

# External object files for target __idf_driver
__idf_driver_EXTERNAL_OBJECTS =

esp-idf/driver/libdriver.a: esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/adc_legacy.c.obj
esp-idf/driver/libdriver.a: esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/adc_dma_legacy.c.obj
esp-idf/driver/libdriver.a: esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/dac_common_legacy.c.obj
esp-idf/driver/libdriver.a: esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/esp32/dac_legacy.c.obj
esp-idf/driver/libdriver.a: esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/timer_legacy.c.obj
esp-idf/driver/libdriver.a: esp-idf/driver/CMakeFiles/__idf_driver.dir/i2c/i2c.c.obj
esp-idf/driver/libdriver.a: esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/i2s_legacy.c.obj
esp-idf/driver/libdriver.a: esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/mcpwm_legacy.c.obj
esp-idf/driver/libdriver.a: esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/pcnt_legacy.c.obj
esp-idf/driver/libdriver.a: esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/rmt_legacy.c.obj
esp-idf/driver/libdriver.a: esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/sigma_delta_legacy.c.obj
esp-idf/driver/libdriver.a: esp-idf/driver/CMakeFiles/__idf_driver.dir/touch_sensor/touch_sensor_common.c.obj
esp-idf/driver/libdriver.a: esp-idf/driver/CMakeFiles/__idf_driver.dir/touch_sensor/esp32/touch_sensor.c.obj
esp-idf/driver/libdriver.a: esp-idf/driver/CMakeFiles/__idf_driver.dir/twai/twai.c.obj
esp-idf/driver/libdriver.a: esp-idf/driver/CMakeFiles/__idf_driver.dir/deprecated/adc_i2s_deprecated.c.obj
esp-idf/driver/libdriver.a: esp-idf/driver/CMakeFiles/__idf_driver.dir/build.make
esp-idf/driver/libdriver.a: esp-idf/driver/CMakeFiles/__idf_driver.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/zeng/share/esp/esp-idf/gateway/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_16) "Linking C static library libdriver.a"
	cd /home/zeng/share/esp/esp-idf/gateway/build/esp-idf/driver && $(CMAKE_COMMAND) -P CMakeFiles/__idf_driver.dir/cmake_clean_target.cmake
	cd /home/zeng/share/esp/esp-idf/gateway/build/esp-idf/driver && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/__idf_driver.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
esp-idf/driver/CMakeFiles/__idf_driver.dir/build: esp-idf/driver/libdriver.a

.PHONY : esp-idf/driver/CMakeFiles/__idf_driver.dir/build

esp-idf/driver/CMakeFiles/__idf_driver.dir/clean:
	cd /home/zeng/share/esp/esp-idf/gateway/build/esp-idf/driver && $(CMAKE_COMMAND) -P CMakeFiles/__idf_driver.dir/cmake_clean.cmake
.PHONY : esp-idf/driver/CMakeFiles/__idf_driver.dir/clean

esp-idf/driver/CMakeFiles/__idf_driver.dir/depend:
	cd /home/zeng/share/esp/esp-idf/gateway/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/zeng/share/esp/esp-idf/gateway /home/zeng/share/esp/esp-idf/esp-idf/components/driver /home/zeng/share/esp/esp-idf/gateway/build /home/zeng/share/esp/esp-idf/gateway/build/esp-idf/driver /home/zeng/share/esp/esp-idf/gateway/build/esp-idf/driver/CMakeFiles/__idf_driver.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : esp-idf/driver/CMakeFiles/__idf_driver.dir/depend

