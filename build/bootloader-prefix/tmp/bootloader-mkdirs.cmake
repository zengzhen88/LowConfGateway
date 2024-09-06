# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/zeng/share/espdemo/esp-idf/components/bootloader/subproject"
  "/home/zeng/share/gateway/LowConfGateway/build/bootloader"
  "/home/zeng/share/gateway/LowConfGateway/build/bootloader-prefix"
  "/home/zeng/share/gateway/LowConfGateway/build/bootloader-prefix/tmp"
  "/home/zeng/share/gateway/LowConfGateway/build/bootloader-prefix/src/bootloader-stamp"
  "/home/zeng/share/gateway/LowConfGateway/build/bootloader-prefix/src"
  "/home/zeng/share/gateway/LowConfGateway/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/zeng/share/gateway/LowConfGateway/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/zeng/share/gateway/LowConfGateway/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
