cmake_minimum_required (VERSION 3.4)

project(khf02)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror")

enable_testing()

add_executable(khf02
  main.cpp
  )

add_test(NAME khf02_basetest
  COMMAND khf02 -v)
