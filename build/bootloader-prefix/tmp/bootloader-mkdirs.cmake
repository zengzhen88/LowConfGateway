# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Espressif5/frameworks/esp-idf-v5.3.1/components/bootloader/subproject"
  "C:/msys64/home/zeng/gateway/LowConfGateway/build/bootloader"
  "C:/msys64/home/zeng/gateway/LowConfGateway/build/bootloader-prefix"
  "C:/msys64/home/zeng/gateway/LowConfGateway/build/bootloader-prefix/tmp"
  "C:/msys64/home/zeng/gateway/LowConfGateway/build/bootloader-prefix/src/bootloader-stamp"
  "C:/msys64/home/zeng/gateway/LowConfGateway/build/bootloader-prefix/src"
  "C:/msys64/home/zeng/gateway/LowConfGateway/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/msys64/home/zeng/gateway/LowConfGateway/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/msys64/home/zeng/gateway/LowConfGateway/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
