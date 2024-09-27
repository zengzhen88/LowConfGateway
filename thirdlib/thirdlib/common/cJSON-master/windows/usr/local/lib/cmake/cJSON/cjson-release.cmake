#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "cjson" for configuration "Release"
set_property(TARGET cjson APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(cjson PROPERTIES
  IMPORTED_IMPLIB_RELEASE "/usr/local/lib/libcjson.dll.a"
  IMPORTED_LOCATION_RELEASE "/usr/local/bin/msys-cjson-1.dll"
  )

list(APPEND _cmake_import_check_targets cjson )
list(APPEND _cmake_import_check_files_for_cjson "/usr/local/lib/libcjson.dll.a" "/usr/local/bin/msys-cjson-1.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
