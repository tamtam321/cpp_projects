cmake_minimum_required (VERSION 3.0)

project(memoryPool)
include_directories(include)

set (CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Werror -Wall -Wextra -pedantic")

set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

set (SOURCES main.cpp)

add_executable (memoryPool ${SOURCES})
