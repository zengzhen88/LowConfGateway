# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.28

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
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
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/zeng/share/gateway/LowConfGateway

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/zeng/share/gateway/LowConfGateway/build

# Include any dependencies generated for this target.
include esp-idf/main/CMakeFiles/__idf_main.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include esp-idf/main/CMakeFiles/__idf_main.dir/compiler_depend.make

# Include the progress variables for this target.
include esp-idf/main/CMakeFiles/__idf_main.dir/progress.make

# Include the compile flags for this target's objects.
include esp-idf/main/CMakeFiles/__idf_main.dir/flags.make

esp-idf/main/CMakeFiles/__idf_main.dir/hello_world_main.c.obj: esp-idf/main/CMakeFiles/__idf_main.dir/flags.make
esp-idf/main/CMakeFiles/__idf_main.dir/hello_world_main.c.obj: /home/zeng/share/gateway/LowConfGateway/main/hello_world_main.c
esp-idf/main/CMakeFiles/__idf_main.dir/hello_world_main.c.obj: esp-idf/main/CMakeFiles/__idf_main.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/zeng/share/gateway/LowConfGateway/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object esp-idf/main/CMakeFiles/__idf_main.dir/hello_world_main.c.obj"
	cd /home/zeng/share/gateway/LowConfGateway/build/esp-idf/main && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT esp-idf/main/CMakeFiles/__idf_main.dir/hello_world_main.c.obj -MF CMakeFiles/__idf_main.dir/hello_world_main.c.obj.d -o CMakeFiles/__idf_main.dir/hello_world_main.c.obj -c /home/zeng/share/gateway/LowConfGateway/main/hello_world_main.c

esp-idf/main/CMakeFiles/__idf_main.dir/hello_world_main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/__idf_main.dir/hello_world_main.c.i"
	cd /home/zeng/share/gateway/LowConfGateway/build/esp-idf/main && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/zeng/share/gateway/LowConfGateway/main/hello_world_main.c > CMakeFiles/__idf_main.dir/hello_world_main.c.i

esp-idf/main/CMakeFiles/__idf_main.dir/hello_world_main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/__idf_main.dir/hello_world_main.c.s"
	cd /home/zeng/share/gateway/LowConfGateway/build/esp-idf/main && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/zeng/share/gateway/LowConfGateway/main/hello_world_main.c -o CMakeFiles/__idf_main.dir/hello_world_main.c.s

esp-idf/main/CMakeFiles/__idf_main.dir/common.c.obj: esp-idf/main/CMakeFiles/__idf_main.dir/flags.make
esp-idf/main/CMakeFiles/__idf_main.dir/common.c.obj: /home/zeng/share/gateway/LowConfGateway/main/common.c
esp-idf/main/CMakeFiles/__idf_main.dir/common.c.obj: esp-idf/main/CMakeFiles/__idf_main.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/zeng/share/gateway/LowConfGateway/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object esp-idf/main/CMakeFiles/__idf_main.dir/common.c.obj"
	cd /home/zeng/share/gateway/LowConfGateway/build/esp-idf/main && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT esp-idf/main/CMakeFiles/__idf_main.dir/common.c.obj -MF CMakeFiles/__idf_main.dir/common.c.obj.d -o CMakeFiles/__idf_main.dir/common.c.obj -c /home/zeng/share/gateway/LowConfGateway/main/common.c

esp-idf/main/CMakeFiles/__idf_main.dir/common.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/__idf_main.dir/common.c.i"
	cd /home/zeng/share/gateway/LowConfGateway/build/esp-idf/main && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/zeng/share/gateway/LowConfGateway/main/common.c > CMakeFiles/__idf_main.dir/common.c.i

esp-idf/main/CMakeFiles/__idf_main.dir/common.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/__idf_main.dir/common.c.s"
	cd /home/zeng/share/gateway/LowConfGateway/build/esp-idf/main && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/zeng/share/gateway/LowConfGateway/main/common.c -o CMakeFiles/__idf_main.dir/common.c.s

esp-idf/main/CMakeFiles/__idf_main.dir/message.c.obj: esp-idf/main/CMakeFiles/__idf_main.dir/flags.make
esp-idf/main/CMakeFiles/__idf_main.dir/message.c.obj: /home/zeng/share/gateway/LowConfGateway/main/message.c
esp-idf/main/CMakeFiles/__idf_main.dir/message.c.obj: esp-idf/main/CMakeFiles/__idf_main.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/zeng/share/gateway/LowConfGateway/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object esp-idf/main/CMakeFiles/__idf_main.dir/message.c.obj"
	cd /home/zeng/share/gateway/LowConfGateway/build/esp-idf/main && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT esp-idf/main/CMakeFiles/__idf_main.dir/message.c.obj -MF CMakeFiles/__idf_main.dir/message.c.obj.d -o CMakeFiles/__idf_main.dir/message.c.obj -c /home/zeng/share/gateway/LowConfGateway/main/message.c

esp-idf/main/CMakeFiles/__idf_main.dir/message.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/__idf_main.dir/message.c.i"
	cd /home/zeng/share/gateway/LowConfGateway/build/esp-idf/main && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/zeng/share/gateway/LowConfGateway/main/message.c > CMakeFiles/__idf_main.dir/message.c.i

esp-idf/main/CMakeFiles/__idf_main.dir/message.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/__idf_main.dir/message.c.s"
	cd /home/zeng/share/gateway/LowConfGateway/build/esp-idf/main && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/zeng/share/gateway/LowConfGateway/main/message.c -o CMakeFiles/__idf_main.dir/message.c.s

esp-idf/main/CMakeFiles/__idf_main.dir/mqtt.c.obj: esp-idf/main/CMakeFiles/__idf_main.dir/flags.make
esp-idf/main/CMakeFiles/__idf_main.dir/mqtt.c.obj: /home/zeng/share/gateway/LowConfGateway/main/mqtt.c
esp-idf/main/CMakeFiles/__idf_main.dir/mqtt.c.obj: esp-idf/main/CMakeFiles/__idf_main.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/zeng/share/gateway/LowConfGateway/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object esp-idf/main/CMakeFiles/__idf_main.dir/mqtt.c.obj"
	cd /home/zeng/share/gateway/LowConfGateway/build/esp-idf/main && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT esp-idf/main/CMakeFiles/__idf_main.dir/mqtt.c.obj -MF CMakeFiles/__idf_main.dir/mqtt.c.obj.d -o CMakeFiles/__idf_main.dir/mqtt.c.obj -c /home/zeng/share/gateway/LowConfGateway/main/mqtt.c

esp-idf/main/CMakeFiles/__idf_main.dir/mqtt.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/__idf_main.dir/mqtt.c.i"
	cd /home/zeng/share/gateway/LowConfGateway/build/esp-idf/main && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/zeng/share/gateway/LowConfGateway/main/mqtt.c > CMakeFiles/__idf_main.dir/mqtt.c.i

esp-idf/main/CMakeFiles/__idf_main.dir/mqtt.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/__idf_main.dir/mqtt.c.s"
	cd /home/zeng/share/gateway/LowConfGateway/build/esp-idf/main && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/zeng/share/gateway/LowConfGateway/main/mqtt.c -o CMakeFiles/__idf_main.dir/mqtt.c.s

esp-idf/main/CMakeFiles/__idf_main.dir/wifi.c.obj: esp-idf/main/CMakeFiles/__idf_main.dir/flags.make
esp-idf/main/CMakeFiles/__idf_main.dir/wifi.c.obj: /home/zeng/share/gateway/LowConfGateway/main/wifi.c
esp-idf/main/CMakeFiles/__idf_main.dir/wifi.c.obj: esp-idf/main/CMakeFiles/__idf_main.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/zeng/share/gateway/LowConfGateway/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building C object esp-idf/main/CMakeFiles/__idf_main.dir/wifi.c.obj"
	cd /home/zeng/share/gateway/LowConfGateway/build/esp-idf/main && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT esp-idf/main/CMakeFiles/__idf_main.dir/wifi.c.obj -MF CMakeFiles/__idf_main.dir/wifi.c.obj.d -o CMakeFiles/__idf_main.dir/wifi.c.obj -c /home/zeng/share/gateway/LowConfGateway/main/wifi.c

esp-idf/main/CMakeFiles/__idf_main.dir/wifi.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/__idf_main.dir/wifi.c.i"
	cd /home/zeng/share/gateway/LowConfGateway/build/esp-idf/main && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/zeng/share/gateway/LowConfGateway/main/wifi.c > CMakeFiles/__idf_main.dir/wifi.c.i

esp-idf/main/CMakeFiles/__idf_main.dir/wifi.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/__idf_main.dir/wifi.c.s"
	cd /home/zeng/share/gateway/LowConfGateway/build/esp-idf/main && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/zeng/share/gateway/LowConfGateway/main/wifi.c -o CMakeFiles/__idf_main.dir/wifi.c.s

esp-idf/main/CMakeFiles/__idf_main.dir/uart.c.obj: esp-idf/main/CMakeFiles/__idf_main.dir/flags.make
esp-idf/main/CMakeFiles/__idf_main.dir/uart.c.obj: /home/zeng/share/gateway/LowConfGateway/main/uart.c
esp-idf/main/CMakeFiles/__idf_main.dir/uart.c.obj: esp-idf/main/CMakeFiles/__idf_main.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/zeng/share/gateway/LowConfGateway/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building C object esp-idf/main/CMakeFiles/__idf_main.dir/uart.c.obj"
	cd /home/zeng/share/gateway/LowConfGateway/build/esp-idf/main && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT esp-idf/main/CMakeFiles/__idf_main.dir/uart.c.obj -MF CMakeFiles/__idf_main.dir/uart.c.obj.d -o CMakeFiles/__idf_main.dir/uart.c.obj -c /home/zeng/share/gateway/LowConfGateway/main/uart.c

esp-idf/main/CMakeFiles/__idf_main.dir/uart.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/__idf_main.dir/uart.c.i"
	cd /home/zeng/share/gateway/LowConfGateway/build/esp-idf/main && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/zeng/share/gateway/LowConfGateway/main/uart.c > CMakeFiles/__idf_main.dir/uart.c.i

esp-idf/main/CMakeFiles/__idf_main.dir/uart.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/__idf_main.dir/uart.c.s"
	cd /home/zeng/share/gateway/LowConfGateway/build/esp-idf/main && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/zeng/share/gateway/LowConfGateway/main/uart.c -o CMakeFiles/__idf_main.dir/uart.c.s

esp-idf/main/CMakeFiles/__idf_main.dir/rbtree.c.obj: esp-idf/main/CMakeFiles/__idf_main.dir/flags.make
esp-idf/main/CMakeFiles/__idf_main.dir/rbtree.c.obj: /home/zeng/share/gateway/LowConfGateway/main/rbtree.c
esp-idf/main/CMakeFiles/__idf_main.dir/rbtree.c.obj: esp-idf/main/CMakeFiles/__idf_main.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/zeng/share/gateway/LowConfGateway/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building C object esp-idf/main/CMakeFiles/__idf_main.dir/rbtree.c.obj"
	cd /home/zeng/share/gateway/LowConfGateway/build/esp-idf/main && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT esp-idf/main/CMakeFiles/__idf_main.dir/rbtree.c.obj -MF CMakeFiles/__idf_main.dir/rbtree.c.obj.d -o CMakeFiles/__idf_main.dir/rbtree.c.obj -c /home/zeng/share/gateway/LowConfGateway/main/rbtree.c

esp-idf/main/CMakeFiles/__idf_main.dir/rbtree.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/__idf_main.dir/rbtree.c.i"
	cd /home/zeng/share/gateway/LowConfGateway/build/esp-idf/main && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/zeng/share/gateway/LowConfGateway/main/rbtree.c > CMakeFiles/__idf_main.dir/rbtree.c.i

esp-idf/main/CMakeFiles/__idf_main.dir/rbtree.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/__idf_main.dir/rbtree.c.s"
	cd /home/zeng/share/gateway/LowConfGateway/build/esp-idf/main && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/zeng/share/gateway/LowConfGateway/main/rbtree.c -o CMakeFiles/__idf_main.dir/rbtree.c.s

esp-idf/main/CMakeFiles/__idf_main.dir/rbtrees.c.obj: esp-idf/main/CMakeFiles/__idf_main.dir/flags.make
esp-idf/main/CMakeFiles/__idf_main.dir/rbtrees.c.obj: /home/zeng/share/gateway/LowConfGateway/main/rbtrees.c
esp-idf/main/CMakeFiles/__idf_main.dir/rbtrees.c.obj: esp-idf/main/CMakeFiles/__idf_main.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/zeng/share/gateway/LowConfGateway/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building C object esp-idf/main/CMakeFiles/__idf_main.dir/rbtrees.c.obj"
	cd /home/zeng/share/gateway/LowConfGateway/build/esp-idf/main && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT esp-idf/main/CMakeFiles/__idf_main.dir/rbtrees.c.obj -MF CMakeFiles/__idf_main.dir/rbtrees.c.obj.d -o CMakeFiles/__idf_main.dir/rbtrees.c.obj -c /home/zeng/share/gateway/LowConfGateway/main/rbtrees.c

esp-idf/main/CMakeFiles/__idf_main.dir/rbtrees.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/__idf_main.dir/rbtrees.c.i"
	cd /home/zeng/share/gateway/LowConfGateway/build/esp-idf/main && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/zeng/share/gateway/LowConfGateway/main/rbtrees.c > CMakeFiles/__idf_main.dir/rbtrees.c.i

esp-idf/main/CMakeFiles/__idf_main.dir/rbtrees.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/__idf_main.dir/rbtrees.c.s"
	cd /home/zeng/share/gateway/LowConfGateway/build/esp-idf/main && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/zeng/share/gateway/LowConfGateway/main/rbtrees.c -o CMakeFiles/__idf_main.dir/rbtrees.c.s

esp-idf/main/CMakeFiles/__idf_main.dir/eth.c.obj: esp-idf/main/CMakeFiles/__idf_main.dir/flags.make
esp-idf/main/CMakeFiles/__idf_main.dir/eth.c.obj: /home/zeng/share/gateway/LowConfGateway/main/eth.c
esp-idf/main/CMakeFiles/__idf_main.dir/eth.c.obj: esp-idf/main/CMakeFiles/__idf_main.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/zeng/share/gateway/LowConfGateway/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building C object esp-idf/main/CMakeFiles/__idf_main.dir/eth.c.obj"
	cd /home/zeng/share/gateway/LowConfGateway/build/esp-idf/main && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT esp-idf/main/CMakeFiles/__idf_main.dir/eth.c.obj -MF CMakeFiles/__idf_main.dir/eth.c.obj.d -o CMakeFiles/__idf_main.dir/eth.c.obj -c /home/zeng/share/gateway/LowConfGateway/main/eth.c

esp-idf/main/CMakeFiles/__idf_main.dir/eth.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/__idf_main.dir/eth.c.i"
	cd /home/zeng/share/gateway/LowConfGateway/build/esp-idf/main && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/zeng/share/gateway/LowConfGateway/main/eth.c > CMakeFiles/__idf_main.dir/eth.c.i

esp-idf/main/CMakeFiles/__idf_main.dir/eth.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/__idf_main.dir/eth.c.s"
	cd /home/zeng/share/gateway/LowConfGateway/build/esp-idf/main && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/zeng/share/gateway/LowConfGateway/main/eth.c -o CMakeFiles/__idf_main.dir/eth.c.s

esp-idf/main/CMakeFiles/__idf_main.dir/update.c.obj: esp-idf/main/CMakeFiles/__idf_main.dir/flags.make
esp-idf/main/CMakeFiles/__idf_main.dir/update.c.obj: /home/zeng/share/gateway/LowConfGateway/main/update.c
esp-idf/main/CMakeFiles/__idf_main.dir/update.c.obj: esp-idf/main/CMakeFiles/__idf_main.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/zeng/share/gateway/LowConfGateway/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building C object esp-idf/main/CMakeFiles/__idf_main.dir/update.c.obj"
	cd /home/zeng/share/gateway/LowConfGateway/build/esp-idf/main && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT esp-idf/main/CMakeFiles/__idf_main.dir/update.c.obj -MF CMakeFiles/__idf_main.dir/update.c.obj.d -o CMakeFiles/__idf_main.dir/update.c.obj -c /home/zeng/share/gateway/LowConfGateway/main/update.c

esp-idf/main/CMakeFiles/__idf_main.dir/update.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/__idf_main.dir/update.c.i"
	cd /home/zeng/share/gateway/LowConfGateway/build/esp-idf/main && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/zeng/share/gateway/LowConfGateway/main/update.c > CMakeFiles/__idf_main.dir/update.c.i

esp-idf/main/CMakeFiles/__idf_main.dir/update.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/__idf_main.dir/update.c.s"
	cd /home/zeng/share/gateway/LowConfGateway/build/esp-idf/main && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/zeng/share/gateway/LowConfGateway/main/update.c -o CMakeFiles/__idf_main.dir/update.c.s

esp-idf/main/CMakeFiles/__idf_main.dir/spi.c.obj: esp-idf/main/CMakeFiles/__idf_main.dir/flags.make
esp-idf/main/CMakeFiles/__idf_main.dir/spi.c.obj: /home/zeng/share/gateway/LowConfGateway/main/spi.c
esp-idf/main/CMakeFiles/__idf_main.dir/spi.c.obj: esp-idf/main/CMakeFiles/__idf_main.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/zeng/share/gateway/LowConfGateway/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Building C object esp-idf/main/CMakeFiles/__idf_main.dir/spi.c.obj"
	cd /home/zeng/share/gateway/LowConfGateway/build/esp-idf/main && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT esp-idf/main/CMakeFiles/__idf_main.dir/spi.c.obj -MF CMakeFiles/__idf_main.dir/spi.c.obj.d -o CMakeFiles/__idf_main.dir/spi.c.obj -c /home/zeng/share/gateway/LowConfGateway/main/spi.c

esp-idf/main/CMakeFiles/__idf_main.dir/spi.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/__idf_main.dir/spi.c.i"
	cd /home/zeng/share/gateway/LowConfGateway/build/esp-idf/main && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/zeng/share/gateway/LowConfGateway/main/spi.c > CMakeFiles/__idf_main.dir/spi.c.i

esp-idf/main/CMakeFiles/__idf_main.dir/spi.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/__idf_main.dir/spi.c.s"
	cd /home/zeng/share/gateway/LowConfGateway/build/esp-idf/main && /home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/zeng/share/gateway/LowConfGateway/main/spi.c -o CMakeFiles/__idf_main.dir/spi.c.s

# Object files for target __idf_main
__idf_main_OBJECTS = \
"CMakeFiles/__idf_main.dir/hello_world_main.c.obj" \
"CMakeFiles/__idf_main.dir/common.c.obj" \
"CMakeFiles/__idf_main.dir/message.c.obj" \
"CMakeFiles/__idf_main.dir/mqtt.c.obj" \
"CMakeFiles/__idf_main.dir/wifi.c.obj" \
"CMakeFiles/__idf_main.dir/uart.c.obj" \
"CMakeFiles/__idf_main.dir/rbtree.c.obj" \
"CMakeFiles/__idf_main.dir/rbtrees.c.obj" \
"CMakeFiles/__idf_main.dir/eth.c.obj" \
"CMakeFiles/__idf_main.dir/update.c.obj" \
"CMakeFiles/__idf_main.dir/spi.c.obj"

# External object files for target __idf_main
__idf_main_EXTERNAL_OBJECTS =

esp-idf/main/libmain.a: esp-idf/main/CMakeFiles/__idf_main.dir/hello_world_main.c.obj
esp-idf/main/libmain.a: esp-idf/main/CMakeFiles/__idf_main.dir/common.c.obj
esp-idf/main/libmain.a: esp-idf/main/CMakeFiles/__idf_main.dir/message.c.obj
esp-idf/main/libmain.a: esp-idf/main/CMakeFiles/__idf_main.dir/mqtt.c.obj
esp-idf/main/libmain.a: esp-idf/main/CMakeFiles/__idf_main.dir/wifi.c.obj
esp-idf/main/libmain.a: esp-idf/main/CMakeFiles/__idf_main.dir/uart.c.obj
esp-idf/main/libmain.a: esp-idf/main/CMakeFiles/__idf_main.dir/rbtree.c.obj
esp-idf/main/libmain.a: esp-idf/main/CMakeFiles/__idf_main.dir/rbtrees.c.obj
esp-idf/main/libmain.a: esp-idf/main/CMakeFiles/__idf_main.dir/eth.c.obj
esp-idf/main/libmain.a: esp-idf/main/CMakeFiles/__idf_main.dir/update.c.obj
esp-idf/main/libmain.a: esp-idf/main/CMakeFiles/__idf_main.dir/spi.c.obj
esp-idf/main/libmain.a: esp-idf/main/CMakeFiles/__idf_main.dir/build.make
esp-idf/main/libmain.a: esp-idf/main/CMakeFiles/__idf_main.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/zeng/share/gateway/LowConfGateway/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_12) "Linking C static library libmain.a"
	cd /home/zeng/share/gateway/LowConfGateway/build/esp-idf/main && $(CMAKE_COMMAND) -P CMakeFiles/__idf_main.dir/cmake_clean_target.cmake
	cd /home/zeng/share/gateway/LowConfGateway/build/esp-idf/main && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/__idf_main.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
esp-idf/main/CMakeFiles/__idf_main.dir/build: esp-idf/main/libmain.a
.PHONY : esp-idf/main/CMakeFiles/__idf_main.dir/build

esp-idf/main/CMakeFiles/__idf_main.dir/clean:
	cd /home/zeng/share/gateway/LowConfGateway/build/esp-idf/main && $(CMAKE_COMMAND) -P CMakeFiles/__idf_main.dir/cmake_clean.cmake
.PHONY : esp-idf/main/CMakeFiles/__idf_main.dir/clean

esp-idf/main/CMakeFiles/__idf_main.dir/depend:
	cd /home/zeng/share/gateway/LowConfGateway/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/zeng/share/gateway/LowConfGateway /home/zeng/share/gateway/LowConfGateway/main /home/zeng/share/gateway/LowConfGateway/build /home/zeng/share/gateway/LowConfGateway/build/esp-idf/main /home/zeng/share/gateway/LowConfGateway/build/esp-idf/main/CMakeFiles/__idf_main.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : esp-idf/main/CMakeFiles/__idf_main.dir/depend

