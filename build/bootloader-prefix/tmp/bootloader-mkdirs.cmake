# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/esp/v5.3/esp-idf/components/bootloader/subproject"
  "D:/RFC_BUILD/RFC_Try/build/bootloader"
  "D:/RFC_BUILD/RFC_Try/build/bootloader-prefix"
  "D:/RFC_BUILD/RFC_Try/build/bootloader-prefix/tmp"
  "D:/RFC_BUILD/RFC_Try/build/bootloader-prefix/src/bootloader-stamp"
  "D:/RFC_BUILD/RFC_Try/build/bootloader-prefix/src"
  "D:/RFC_BUILD/RFC_Try/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/RFC_BUILD/RFC_Try/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "D:/RFC_BUILD/RFC_Try/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
