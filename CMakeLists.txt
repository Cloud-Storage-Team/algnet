cmake_minimum_required(VERSION 3.22.0)
set(CMAKE_CXX_STANDARD 20)

project(simulator)

# main part of project

option(BUILD_PROJECT "Build tests" ON)

file(GLOB_RECURSE src "source/*.cpp")
file(GLOB_RECURSE parse "topology_parser/*.cpp")
file(GLOB_RECURSE tests "test/*.cpp")

add_library(
        parser ${parse}
)
add_subdirectory(${CMAKE_SOURCE_DIR}/yaml-cpp)
#find_package(${CMAKE_SOURCE_DIR}/yaml-cpp)
include_directories(${CMAKE_SOURCE_DIR}/spdlog/include/)
include_directories(${CMAKE_SOURCE_DIR}/yaml-cpp/include/)

if (BUILD_PROJECT)
    set(TARGET_NAME "${PROJECT_NAME}")
    target_link_libraries(parser PUBLIC yaml-cpp::yaml-cpp)
    add_executable(${TARGET_NAME} main.cpp ${src})
endif()

# tests
option(BUILD_TESTS "Build tests" ON)

if(BUILD_TESTS)
    add_subdirectory(googletest)

    set(TEST_NAME "test_simulator")
    add_executable(${TEST_NAME} ${src} ${tests})
    target_include_directories(${TEST_NAME} PUBLIC "source")
    target_link_libraries(${TEST_NAME} gtest gtest_main)
    enable_testing()
    add_test(NAME MyTests COMMAND ${TEST_NAME})
endif()
