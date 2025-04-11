cmake_minimum_required(VERSION 3.12)
project(network_simulator)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE src "../source/*.cpp")

# Dependencies
#find_package(yaml-cpp REQUIRED)
add_subdirectory(../yaml-cpp)
#
#include_directories(${CMAKE_SOURCE_DIR}/../spdlog/include/)

# Add all simulator source files (adjust as needed)
add_library(simulator
        ../source/device.cpp ../source/link.cpp ../source/event.cpp
        ../source/packet.cpp ../source/scheduler.cpp
        ../source/switch.cpp
)

# Main executable
add_executable(network_sim
        main.cpp
        parser.cpp
)

target_link_libraries(network_sim
        PRIVATE
        simulator
        yaml-cpp
#        spdlog::spdlog  # If spdlog is used
)