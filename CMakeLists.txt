
cmake_minimum_required(VERSION 3.27)

project(bblib)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_options(
    -Wall
    -Werror
    -fdiagnostics-color=always)
    
add_compile_options(
    "$<$<CONFIG:DEBUG>:-fsanitize=address>")

add_link_options(
    "$<$<CONFIG:DEBUG>:-fsanitize=address>")

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

enable_testing()

find_package(Boost)
find_package(GTest)

add_subdirectory(executor)

