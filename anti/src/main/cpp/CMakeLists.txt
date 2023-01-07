cmake_minimum_required(VERSION 3.18.1)

project("anti")

add_subdirectory(mini_io)

include_directories(mini_io
        JNIHelper
        check
        utils)

add_library(
        anti

        SHARED

        anti.cpp
        check/anti_frida.cpp
        check/anti_frida.h
        check/anti_dual_app.cpp
        check/anti_dual_app.h
        check/anti_xposed.cpp
        check/anti_xposed.h
        check/anti_emulator.cpp
        check/anti_emulator.h
        check/anti_mem_dump.cpp
        check/anti_mem_dump.h

        JNIHelper/utils/JavaObjectPointer.cpp
        JNIHelper/utils/JStringUtils.cpp
        JNIHelper/native/JavaNativeMethod.cpp
        JNIHelper/core/JNIEnvironment.cpp
        JNIHelper/core/JavaCustomClass.cpp

        utils/Utils.cpp

        xposeddetector/art.h
        xposeddetector/classloader.cpp
        xposeddetector/classloader.h
        xposeddetector/find_name.cpp
        xposeddetector/find_name.h
        xposeddetector/hash.cpp
        xposeddetector/hash.h
        xposeddetector/plt.h
        xposeddetector/plt.c
        xposeddetector/xposed.cpp
        xposeddetector/xposed.h
)


find_library(
        log-lib
        log)

target_link_libraries(
        anti

        mini_io
        ${log-lib})