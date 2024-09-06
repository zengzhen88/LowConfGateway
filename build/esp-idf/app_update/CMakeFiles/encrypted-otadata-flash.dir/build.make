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

# Utility rule file for encrypted-otadata-flash.

# Include any custom commands dependencies for this target.
include esp-idf/app_update/CMakeFiles/encrypted-otadata-flash.dir/compiler_depend.make

# Include the progress variables for this target.
include esp-idf/app_update/CMakeFiles/encrypted-otadata-flash.dir/progress.make

esp-idf/app_update/CMakeFiles/encrypted-otadata-flash:
	cd /home/zeng/share/gateway/LowConfGateway/build/esp-idf/app_update && /usr/bin/cmake -E echo "Error: The target encrypted-otadata-flash requires"
	cd /home/zeng/share/gateway/LowConfGateway/build/esp-idf/app_update && /usr/bin/cmake -E echo "CONFIG_SECURE_FLASH_ENCRYPTION_MODE_DEVELOPMENT to be enabled."
	cd /home/zeng/share/gateway/LowConfGateway/build/esp-idf/app_update && /usr/bin/cmake -E env "FAIL_MESSAGE=Failed executing target (see errors on lines above)" /usr/bin/cmake -P /home/zeng/share/espdemo/esp-idf/tools/cmake/scripts/fail.cmake

encrypted-otadata-flash: esp-idf/app_update/CMakeFiles/encrypted-otadata-flash
encrypted-otadata-flash: esp-idf/app_update/CMakeFiles/encrypted-otadata-flash.dir/build.make
.PHONY : encrypted-otadata-flash

# Rule to build all files generated by this target.
esp-idf/app_update/CMakeFiles/encrypted-otadata-flash.dir/build: encrypted-otadata-flash
.PHONY : esp-idf/app_update/CMakeFiles/encrypted-otadata-flash.dir/build

esp-idf/app_update/CMakeFiles/encrypted-otadata-flash.dir/clean:
	cd /home/zeng/share/gateway/LowConfGateway/build/esp-idf/app_update && $(CMAKE_COMMAND) -P CMakeFiles/encrypted-otadata-flash.dir/cmake_clean.cmake
.PHONY : esp-idf/app_update/CMakeFiles/encrypted-otadata-flash.dir/clean

esp-idf/app_update/CMakeFiles/encrypted-otadata-flash.dir/depend:
	cd /home/zeng/share/gateway/LowConfGateway/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/zeng/share/gateway/LowConfGateway /home/zeng/share/espdemo/esp-idf/components/app_update /home/zeng/share/gateway/LowConfGateway/build /home/zeng/share/gateway/LowConfGateway/build/esp-idf/app_update /home/zeng/share/gateway/LowConfGateway/build/esp-idf/app_update/CMakeFiles/encrypted-otadata-flash.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : esp-idf/app_update/CMakeFiles/encrypted-otadata-flash.dir/depend

