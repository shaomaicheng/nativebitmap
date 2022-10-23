#project(nativeBitmap)
cmake_minimum_required(VERSION 3.4.1)

set(FILE_DIR file/native_bitmap.cpp file/native_bitmap.h)
set(BASE base/base.h)
#set(SKIA include/)

include_directories("${CMAKE_SOURCE_DIR}")
aux_source_directory(${CMAKE_SOURCE_DIR} SRC_LIST)


add_library( # Specifies the name of the library.
        nativeBitmap

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        entry.cpp
        ${FILE_DIR}
        )
add_library(include SHARED IMPORTED)
set_target_properties(include PROPERTIES IMPORTED_LOCATION
        "/Users/chenglei01/code/demo/nativeBitmap/nativebitmap/libs/libskia.so")


target_include_directories(nativeBitmap PUBLIC
        "${CMAKE_SOURCE_DIR}"

        "${CMAKE_SOURCE_DIR}/file",
        "${CMAKE_SOURCE_DIR}/base",

        "${CMAKE_SOURCE_DIR}/include"
        "${CMAKE_SOURCE_DIR}/include/android"
        "${CMAKE_SOURCE_DIR}/include/atlastext"
        "${CMAKE_SOURCE_DIR}/include/c"
        "${CMAKE_SOURCE_DIR}/include/codec"
        "${CMAKE_SOURCE_DIR}/include/config"
        "${CMAKE_SOURCE_DIR}/include/core"
        "${CMAKE_SOURCE_DIR}/include/effects"
        "${CMAKE_SOURCE_DIR}/include/pathops"
        "${CMAKE_SOURCE_DIR}/include/ports"
        "${CMAKE_SOURCE_DIR}/include/encode"
        "${CMAKE_SOURCE_DIR}/include/gpu"
        "${CMAKE_SOURCE_DIR}/include/private"
        "${CMAKE_SOURCE_DIR}/include/svg"
        "${CMAKE_SOURCE_DIR}/include/ports"
        "${CMAKE_SOURCE_DIR}/include/third_party"
        "${CMAKE_SOURCE_DIR}/include/utils")

find_library(
        log-lib
        log
)

find_package(shadowhook REQUIRED CONFIG)
find_package(bytehook REQUIRED CONFIG)

target_link_libraries(nativeBitmap bytehook::bytehook)
target_link_libraries(nativeBitmap shadowhook::shadowhook)


target_link_libraries(
        nativeBitmap
        ${log-lib}
)


include_directories(
        file/
        base/
        include/
)