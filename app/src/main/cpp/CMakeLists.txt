
cmake_minimum_required(VERSION 3.6)
#set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  ${WARNING_FLAGS} -D__ARM__ -std=gnu11 -pthread")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WARNING_FLAGS} -D__ARM__ -std=gnu++14 -pthread ")

add_library( # Sets the name of the library.
        native-lib

        SHARED

        native-lib.cpp
        )


#add_subdirectory(xhook)
#include_directories(xhook)

add_subdirectory(linker)
include_directories(linker)

find_library(
        log-lib
        log)


target_link_libraries(
        native-lib linker
        ${log-lib})