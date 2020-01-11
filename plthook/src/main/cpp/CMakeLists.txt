
cmake_minimum_required(VERSION 3.6)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  ${WARNING_FLAGS}  -std=gnu11 -pthread -marm ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WARNING_FLAGS} -std=c++14 -pthread -fexceptions ")


if (${ANDROID_ABI} STREQUAL "x86")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WARNING_FLAGS} -std=c++14 -pthread  -D__X86__")
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WARNING_FLAGS} -std=c++14 -pthread -D__ARM__ ")
endif ()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(plthooks)
include_directories(cjni)
include_directories(linker)
include_directories(sigmux)
include_directories(sig_safe_write)
include_directories(fb)
include_directories(build)
include_directories(breakpad)
include_directories(lss)

set(hooksrc
        plthooks/plthooks.cpp
        plthooks/hooks.cpp
        plthooks/trampoline.cpp)

if (${ANDROID_ABI} STREQUAL "x86")
    list(APPEND hooksrc plthooks/trampoline_x86.c)
elseif(${ANDROID_ABI} STREQUAL "armeabi-v7a")
    list(APPEND hooksrc plthooks/trampoline_arm.c)
elseif(${ANDROID_ABI} STREQUAL "arm64-v8a")
    list(APPEND hooksrc plthooks/trampoline_arm64.c)
endif()

add_library(breakpad
        breakpad/abort_with_reason.c
        )

add_library(fb
        fb/log.cpp
        )
add_library(build
        build/build.cpp
        )

add_library(
        sigmux
        SHARED
        sigmux/phaser.c
        sigmux/sigmux.c
        )

add_library(
        cjni
        SHARED
        cjni/exceptions.c
)


add_library(
        plthooks
        SHARED
        ${hooksrc}
        )


add_library(
        linker
        SHARED
        linker/elfSharedLibData.cpp
        linker/fblink.cpp
        linker/linker.cpp
        linker/sharedlibs.cpp
)
add_library(
        sig_safe_write
        SHARED
        sig_safe_write/sig_safe_write.c
        )

add_library(
        hook
        SHARED
        Atrace.cpp
        hooks.cpp
        jni.cpp
        )

find_library(
        log-lib
        log)

target_link_libraries(
        sig_safe_write sigmux
)

target_link_libraries(breakpad fb)

target_link_libraries(
        cjni
        ${log-lib})

target_link_libraries(
        plthooks cjni sigmux sig_safe_write linker breakpad
        ${log-lib})


target_compile_options(fb PUBLIC -frtti -fexceptions -fno-omit-frame-pointer -ffunction-sections -DDISABLE_CPUCAP -DDISABLE_XPLAT)

target_compile_options(breakpad PUBLIC -Wno-unused-variable -Wno-unused-function -Wno-unused-private-field -Wno-reorder -Wno-empty-body -Wno-deprecated-register -Wno-format-security)

target_link_libraries(
        linker build
        ${log-lib})

target_link_libraries(
        hook plthooks
        ${log-lib})