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

# Utility rule file for erase-otadata.

# Include any custom commands dependencies for this target.
include esp-idf/app_update/CMakeFiles/erase-otadata.dir/compiler_depend.make

# Include the progress variables for this target.
include esp-idf/app_update/CMakeFiles/erase-otadata.dir/progress.make

esp-idf/app_update/CMakeFiles/erase-otadata: /home/zeng/share/espdemo/esp-idf/components/partition_table/partitions_two_ota.csv
	/usr/bin/cmake -D IDF_PATH=/home/zeng/share/espdemo/esp-idf -D "SERIAL_TOOL=/home/zeng/.espressif/python_env/idf5.4_py3.12_env/bin/python;/home/zeng/share/espdemo/esp-idf/components/app_update/otatool.py" -D "SERIAL_TOOL_ARGS=--esptool-args;before=default_reset;after=hard_reset;--partition-table-file;/home/zeng/share/espdemo/esp-idf/components/partition_table/partitions_two_ota.csv;--partition-table-offset;0x8000;erase_otadata" -D WORKING_DIRECTORY=/home/zeng/share/gateway/LowConfGateway/build -P /home/zeng/share/espdemo/esp-idf/components/esptool_py/run_serial_tool.cmake

erase-otadata: esp-idf/app_update/CMakeFiles/erase-otadata
erase-otadata: esp-idf/app_update/CMakeFiles/erase-otadata.dir/build.make
.PHONY : erase-otadata

# Rule to build all files generated by this target.
esp-idf/app_update/CMakeFiles/erase-otadata.dir/build: erase-otadata
.PHONY : esp-idf/app_update/CMakeFiles/erase-otadata.dir/build

esp-idf/app_update/CMakeFiles/erase-otadata.dir/clean:
	cd /home/zeng/share/gateway/LowConfGateway/build/esp-idf/app_update && $(CMAKE_COMMAND) -P CMakeFiles/erase-otadata.dir/cmake_clean.cmake
.PHONY : esp-idf/app_update/CMakeFiles/erase-otadata.dir/clean

esp-idf/app_update/CMakeFiles/erase-otadata.dir/depend:
	cd /home/zeng/share/gateway/LowConfGateway/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/zeng/share/gateway/LowConfGateway /home/zeng/share/espdemo/esp-idf/components/app_update /home/zeng/share/gateway/LowConfGateway/build /home/zeng/share/gateway/LowConfGateway/build/esp-idf/app_update /home/zeng/share/gateway/LowConfGateway/build/esp-idf/app_update/CMakeFiles/erase-otadata.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : esp-idf/app_update/CMakeFiles/erase-otadata.dir/depend
