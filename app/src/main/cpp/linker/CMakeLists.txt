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
        )

add_library(linker
        STATIC
        ${UTILS_SOURCE})
target_link_libraries(linker)