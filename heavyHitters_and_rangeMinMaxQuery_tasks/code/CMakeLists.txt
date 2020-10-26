cmake_minimum_required (VERSION 3.4)

project(khf03)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror")

add_executable(heavyhitters
  main.cpp
  )

