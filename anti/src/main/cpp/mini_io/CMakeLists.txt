 
cmake_minimum_required(VERSION 3.10)
project(mini_io C ASM CXX)

set(CMAKE_C_VISIBILITY_PRESET hidden)
add_library(mini_io STATIC
    _open.c
    _set_errno.c
    _arm_on_x86.c
    syscall/private/common.c
    syscall/${CMAKE_ANDROID_ARCH_ABI}.S
)
