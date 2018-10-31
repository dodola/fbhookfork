cmake_minimum_required(VERSION 3.6)

set(UTILS_SOURCE
        elfSharedLibData.cpp
        hooks.cpp
        link.cpp
        linker.cpp
        phaser.c
        sharedlibs.cpp
        sig_safe_write.c
        sigmux.c
        trampoline.cpp
        abort_with_reason.c
        )

if (${ANDROID_ABI} STREQUAL "x86")
    set(TRAMPOLINE trampoline_x86.c)
else ()
    set(TRAMPOLINE trampoline_arm.c)
endif ()

message(${TRAMPOLINE})
add_library(linker
        STATIC
        ${UTILS_SOURCE}
        ${TRAMPOLINE}
        )
target_link_libraries(linker)