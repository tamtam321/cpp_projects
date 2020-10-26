############################################################
#   base
############################################################

cmake_minimum_required (VERSION 3.14)

############################################################
#   setting Project informations
############################################################

set(PROJECT_NAME interval_tree)

############################################################
#   generate makefiles
############################################################

project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror")

add_executable(${PROJECT_NAME}
  main.cpp
  exceptions.hpp)



