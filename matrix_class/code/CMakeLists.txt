cmake_minimum_required(VERSION 3.10)
project(AdatAlg_khf01)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")

option(USE_WERROR "turn on -Werror flag" ON)
if (USE_WERROR)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
endif (USE_WERROR)

add_executable(AdatAlg_khf01 main.cpp matrix.hpp)