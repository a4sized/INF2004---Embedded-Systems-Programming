# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Users/reggi/Programming/INF2004 - Embedded Systems Programming/Robotic_Car/build/_deps/picotool-src/enc_bootloader"
  "C:/Users/reggi/Programming/INF2004 - Embedded Systems Programming/Robotic_Car/build/_deps/picotool-build/enc_bootloader"
  "C:/Users/reggi/Programming/INF2004 - Embedded Systems Programming/Robotic_Car/build/_deps/picotool-build/enc_bootloader"
  "C:/Users/reggi/Programming/INF2004 - Embedded Systems Programming/Robotic_Car/build/_deps/picotool-build/enc_bootloader/tmp"
  "C:/Users/reggi/Programming/INF2004 - Embedded Systems Programming/Robotic_Car/build/_deps/picotool-build/enc_bootloader/src/enc_bootloader-stamp"
  "C:/Users/reggi/Programming/INF2004 - Embedded Systems Programming/Robotic_Car/build/_deps/picotool-build/enc_bootloader/src"
  "C:/Users/reggi/Programming/INF2004 - Embedded Systems Programming/Robotic_Car/build/_deps/picotool-build/enc_bootloader/src/enc_bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/reggi/Programming/INF2004 - Embedded Systems Programming/Robotic_Car/build/_deps/picotool-build/enc_bootloader/src/enc_bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Users/reggi/Programming/INF2004 - Embedded Systems Programming/Robotic_Car/build/_deps/picotool-build/enc_bootloader/src/enc_bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
