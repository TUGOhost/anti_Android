# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.18.1)

# Declares and names the project.

project("anti")

add_subdirectory(mini_io)

include_directories(mini_io
        JNIHelper/**
        check
        utils)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
        anti

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
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

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        anti

        mini_io
        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})