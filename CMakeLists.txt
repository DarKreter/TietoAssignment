cmake_minimum_required(VERSION 3.10)

project(TietoAssignment LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(DEFINED ENV{CC})
    if("$ENV{CC}" STREQUAL "gcc")
        message(STATUS "GCC compiler is selected")
        set(CPU_OPTIONS -Werror -Wall -Wextra -pedantic -ffunction-sections -fdata-sections -Ofast)
    elseif("$ENV{CC}" STREQUAL "clang")
        message(STATUS "Clang compiler is selected")
        set(CPU_OPTIONS -Weverything)
    else()
        message(FATAL_ERROR "Other compiler is selected ($ENV{CC})")
    endif()
else()
    message(FATAL_ERROR "CC environment variable is not defined")
endif()

add_compile_options(${CPU_OPTIONS})

find_package(Threads REQUIRED)

add_subdirectory(CPU)
add_subdirectory(Threads)
add_subdirectory(firmware)