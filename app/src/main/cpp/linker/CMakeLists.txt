cmake_minimum_required(VERSION 3.6)

set(UTILS_SOURCE
        linker.cpp
        linker.hpp
        phaser.c
        sig_safe_write.c
        sigmux.c
        hooks.cpp
        )

add_library(linker
        STATIC
        ${UTILS_SOURCE})
target_link_libraries(linker)