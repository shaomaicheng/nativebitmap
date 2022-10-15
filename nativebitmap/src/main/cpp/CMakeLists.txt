cmake_minimum_required(VERSION 3.4.1)

set(FILE_DIR file/native_bitmap.cpp file/native_bitmap.h)
set(BASE base/base.h)

add_library( # Specifies the name of the library.
        nativeBitmap

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        entry.cpp
        ${FILE_DIR}
        )

find_library(
        log-lib
        log
)

find_package(bytehook REQUIRED CONFIG)

target_link_libraries(nativeBitmap bytehook::bytehook)



target_link_libraries(
        nativeBitmap
        ${log-lib}
)

include_directories(
        file/
        base/
)