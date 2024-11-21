cmake_minimum_required(VERSION 3.22)
project(express_pass)

set(CMAKE_CXX_STANDARD 20)
file(GLOB SOURCES src/*.cpp)

add_executable(express_pass ${SOURCES})

target_include_directories(express_pass PUBLIC include)

add_subdirectory(tests)