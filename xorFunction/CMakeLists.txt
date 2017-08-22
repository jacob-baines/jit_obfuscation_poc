project(xorFunction CXX)
cmake_minimum_required(VERSION 3.2)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -g -std=c++11")

add_executable(${PROJECT_NAME} src/xorFunction.cpp)

