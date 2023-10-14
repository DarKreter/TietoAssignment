cmake_minimum_required(VERSION 3.10)

project(TietoAssignment LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)


set(CPU_OPTIONS -Werror -Wall -Wextra -pedantic -ffunction-sections -fdata-sections -Ofast)
add_compile_options(${CPU_OPTIONS})


add_subdirectory(CPU)
add_subdirectory(firmware)