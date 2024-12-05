cmake_minimum_required(VERSION 3.22)
project(bfs_simulation)

set(CMAKE_CXX_STANDARD 20)
file(GLOB SOURCES src/*.cpp)

add_executable(bfs_simulation ${SOURCES})

target_include_directories(bfs_simulation PUBLIC include)

add_subdirectory(tests)