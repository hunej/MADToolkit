# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/Users/27zig/esp/esp-idf/components/bootloader/subproject"
  "/Users/27zig/Documents/GitHub/Milsig-Dartsoft-Arduino-Toolkit/esp32/blink/build/bootloader"
  "/Users/27zig/Documents/GitHub/Milsig-Dartsoft-Arduino-Toolkit/esp32/blink/build/bootloader-prefix"
  "/Users/27zig/Documents/GitHub/Milsig-Dartsoft-Arduino-Toolkit/esp32/blink/build/bootloader-prefix/tmp"
  "/Users/27zig/Documents/GitHub/Milsig-Dartsoft-Arduino-Toolkit/esp32/blink/build/bootloader-prefix/src/bootloader-stamp"
  "/Users/27zig/Documents/GitHub/Milsig-Dartsoft-Arduino-Toolkit/esp32/blink/build/bootloader-prefix/src"
  "/Users/27zig/Documents/GitHub/Milsig-Dartsoft-Arduino-Toolkit/esp32/blink/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/27zig/Documents/GitHub/Milsig-Dartsoft-Arduino-Toolkit/esp32/blink/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
