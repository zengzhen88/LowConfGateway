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

# Utility rule file for sections.ld.in.

# Include the progress variables for this target.
include esp-idf/esp_system/CMakeFiles/sections.ld.in.dir/progress.make

esp-idf/esp_system/CMakeFiles/sections.ld.in: esp-idf/esp_system/ld/sections.ld.in


esp-idf/esp_system/ld/sections.ld.in: config/sdkconfig.h
esp-idf/esp_system/ld/sections.ld.in: /home/zeng/share/esp/esp-idf/esp-idf/components/esp_system/ld/esp32/sections.ld.in
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/zeng/share/esp/esp-idf/gateway/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Generating /home/zeng/share/esp/esp-idf/gateway/build/esp-idf/esp_system/ld/sections.ld.in linker script..."
	cd /home/zeng/share/esp/esp-idf/gateway/build/esp-idf/esp_system && /usr/bin/cmake -DCC=/home/zeng/.espressif/tools/xtensa-esp-elf/esp-13.2.0_20240530/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc -DSOURCE=/home/zeng/share/esp/esp-idf/esp-idf/components/esp_system/ld/esp32/sections.ld.in -DTARGET=/home/zeng/share/esp/esp-idf/gateway/build/esp-idf/esp_system/ld/sections.ld.in -DCONFIG_DIR=/home/zeng/share/esp/esp-idf/gateway/build/config -DLD_DIR=/home/zeng/share/esp/esp-idf/esp-idf/components/esp_system/ld -P /home/zeng/share/esp/esp-idf/gateway/build/esp-idf/esp_system/ld/linker_script_generator.cmake

sections.ld.in: esp-idf/esp_system/CMakeFiles/sections.ld.in
sections.ld.in: esp-idf/esp_system/ld/sections.ld.in
sections.ld.in: esp-idf/esp_system/CMakeFiles/sections.ld.in.dir/build.make

.PHONY : sections.ld.in

# Rule to build all files generated by this target.
esp-idf/esp_system/CMakeFiles/sections.ld.in.dir/build: sections.ld.in

.PHONY : esp-idf/esp_system/CMakeFiles/sections.ld.in.dir/build

esp-idf/esp_system/CMakeFiles/sections.ld.in.dir/clean:
	cd /home/zeng/share/esp/esp-idf/gateway/build/esp-idf/esp_system && $(CMAKE_COMMAND) -P CMakeFiles/sections.ld.in.dir/cmake_clean.cmake
.PHONY : esp-idf/esp_system/CMakeFiles/sections.ld.in.dir/clean

esp-idf/esp_system/CMakeFiles/sections.ld.in.dir/depend:
	cd /home/zeng/share/esp/esp-idf/gateway/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/zeng/share/esp/esp-idf/gateway /home/zeng/share/esp/esp-idf/esp-idf/components/esp_system /home/zeng/share/esp/esp-idf/gateway/build /home/zeng/share/esp/esp-idf/gateway/build/esp-idf/esp_system /home/zeng/share/esp/esp-idf/gateway/build/esp-idf/esp_system/CMakeFiles/sections.ld.in.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : esp-idf/esp_system/CMakeFiles/sections.ld.in.dir/depend

