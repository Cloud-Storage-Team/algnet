cmake_minimum_required(VERSION 3.22.0)
set(CMAKE_CXX_STANDARD 20)

project(simulator)

set(TARGET_NAME "${PROJECT_NAME}")

file(GLOB_RECURSE src  "src/*.cpp")

add_executable(${TARGET_NAME} main.cpp ${src})
