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
CMAKE_SOURCE_DIR = /home/zeng/share/esp/esp-idf/esp-idf/components/bootloader/subproject

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/zeng/share/esp/esp-idf/gateway/build/bootloader

# Utility rule file for merge-bin.

# Include the progress variables for this target.
include CMakeFiles/merge-bin.dir/progress.make

CMakeFiles/merge-bin: /home/zeng/share/esp/esp-idf/esp-idf/components/bootloader/subproject/bootloader
	cd /home/zeng/share/esp/esp-idf/esp-idf/components/esptool_py && /usr/bin/cmake -D IDF_PATH=/home/zeng/share/esp/esp-idf/esp-idf -D "SERIAL_TOOL=/home/zeng/.espressif/python_env/idf5.4_py3.8_env/bin/python;;/home/zeng/share/esp/esp-idf/esp-idf/components/esptool_py/esptool/esptool.py;--chip;esp32" -D "SERIAL_TOOL_ARGS=merge_bin;-o;/home/zeng/share/esp/esp-idf/gateway/build/bootloader/merged-binary.bin;@/home/zeng/share/esp/esp-idf/gateway/build/bootloader/flash_args" -D WORKING_DIRECTORY=/home/zeng/share/esp/esp-idf/gateway/build/bootloader -P run_serial_tool.cmake

merge-bin: CMakeFiles/merge-bin
merge-bin: CMakeFiles/merge-bin.dir/build.make

.PHONY : merge-bin

# Rule to build all files generated by this target.
CMakeFiles/merge-bin.dir/build: merge-bin

.PHONY : CMakeFiles/merge-bin.dir/build

CMakeFiles/merge-bin.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/merge-bin.dir/cmake_clean.cmake
.PHONY : CMakeFiles/merge-bin.dir/clean

CMakeFiles/merge-bin.dir/depend:
	cd /home/zeng/share/esp/esp-idf/gateway/build/bootloader && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/zeng/share/esp/esp-idf/esp-idf/components/bootloader/subproject /home/zeng/share/esp/esp-idf/esp-idf/components/bootloader/subproject /home/zeng/share/esp/esp-idf/gateway/build/bootloader /home/zeng/share/esp/esp-idf/gateway/build/bootloader /home/zeng/share/esp/esp-idf/gateway/build/bootloader/CMakeFiles/merge-bin.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/merge-bin.dir/depend

