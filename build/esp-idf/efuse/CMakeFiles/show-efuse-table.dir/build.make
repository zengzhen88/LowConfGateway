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

# Utility rule file for show-efuse-table.

# Include any custom commands dependencies for this target.
include esp-idf/efuse/CMakeFiles/show-efuse-table.dir/compiler_depend.make

# Include the progress variables for this target.
include esp-idf/efuse/CMakeFiles/show-efuse-table.dir/progress.make

esp-idf/efuse/CMakeFiles/show-efuse-table:
	cd /home/zeng/share/gateway/LowConfGateway/build/esp-idf/efuse && /home/zeng/.espressif/python_env/idf5.4_py3.12_env/bin/python /home/zeng/share/espdemo/esp-idf/components/efuse/efuse_table_gen.py /home/zeng/share/espdemo/esp-idf/components/efuse/esp32/esp_efuse_table.csv -t esp32 --max_blk_len 192 --info

show-efuse-table: esp-idf/efuse/CMakeFiles/show-efuse-table
show-efuse-table: esp-idf/efuse/CMakeFiles/show-efuse-table.dir/build.make
.PHONY : show-efuse-table

# Rule to build all files generated by this target.
esp-idf/efuse/CMakeFiles/show-efuse-table.dir/build: show-efuse-table
.PHONY : esp-idf/efuse/CMakeFiles/show-efuse-table.dir/build

esp-idf/efuse/CMakeFiles/show-efuse-table.dir/clean:
	cd /home/zeng/share/gateway/LowConfGateway/build/esp-idf/efuse && $(CMAKE_COMMAND) -P CMakeFiles/show-efuse-table.dir/cmake_clean.cmake
.PHONY : esp-idf/efuse/CMakeFiles/show-efuse-table.dir/clean

esp-idf/efuse/CMakeFiles/show-efuse-table.dir/depend:
	cd /home/zeng/share/gateway/LowConfGateway/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/zeng/share/gateway/LowConfGateway /home/zeng/share/espdemo/esp-idf/components/efuse /home/zeng/share/gateway/LowConfGateway/build /home/zeng/share/gateway/LowConfGateway/build/esp-idf/efuse /home/zeng/share/gateway/LowConfGateway/build/esp-idf/efuse/CMakeFiles/show-efuse-table.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : esp-idf/efuse/CMakeFiles/show-efuse-table.dir/depend
