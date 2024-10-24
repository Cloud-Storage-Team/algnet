cmake_minimum_required(VERSION 3.26)
project(datacenter_overloading)

set(CMAKE_CXX_STANDARD 20)

add_executable(datacenter_overloading src/main.cpp src/simulator.cpp src/utils.cpp)

target_include_directories(datacenter_overloading PUBLIC include)