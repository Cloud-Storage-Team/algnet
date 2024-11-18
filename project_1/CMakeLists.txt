cmake_minimum_required(VERSION 3.26)
project(datacenter_overloading)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(datacenter_overloading ${SOURCES})

target_include_directories(datacenter_overloading PUBLIC include)