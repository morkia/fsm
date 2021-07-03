cmake_minimum_required(VERSION 3.5)

project(fsmv2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if (MSVC)
    # all warnings as errors
    add_compile_options(/WX)
else()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

add_executable(fsmv2 main.cpp fsm.h)
