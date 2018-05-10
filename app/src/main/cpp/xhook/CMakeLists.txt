cmake_minimum_required(VERSION 3.4.1)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  ${WARNING_FLAGS} -D__ARM__ -Wall -Wextra -Werror -fvisibility=hidden -std=c11 -pthread")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WARNING_FLAGS} -D__ARM__  -std=gnu++11 -pthread ")

add_library(
        xhooklib
        STATIC
        xhook.c
        xh_core.c
        xh_elf.c
        xh_jni.c
        xh_log.c
        xh_util.c
        xh_version.c
)


find_library(
        log-lib
        log)

target_link_libraries(
        xhooklib
        ${log-lib})