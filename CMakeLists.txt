cmake_minimum_required(VERSION 3.22.0)
set(CMAKE_CXX_STANDARD 20)

project(simulator)

# main part of project

option(BUILD_PROJECT "Build tests" ON)

file(GLOB_RECURSE src "source/*.cpp")
file(GLOB_RECURSE tests "test/*.cpp")

include_directories(
    ${CMAKE_SOURCE_DIR}/source/
    ${CMAKE_SOURCE_DIR}/spdlog/include/
)

add_subdirectory(${CMAKE_SOURCE_DIR}/yaml-cpp/)

if (BUILD_PROJECT)
    set(TARGET_NAME "${PROJECT_NAME}")
    add_executable(${TARGET_NAME} main.cpp ${src})
    target_link_libraries(${TARGET_NAME} yaml-cpp)
    target_compile_options(${TARGET_NAME} PRIVATE -Wall -Werror -Wextra)
endif()

# tests
option(BUILD_TESTS "Build tests" ON)

if(BUILD_TESTS)
    add_subdirectory(googletest)

    set(TEST_NAME "test_simulator")
    add_executable(${TEST_NAME} ${src} ${tests})
    target_compile_options(${TEST_NAME} PRIVATE -Wall -Wextra)
    target_include_directories(${TEST_NAME} PUBLIC "source")
    target_link_libraries(${TEST_NAME} gtest gtest_main)
    target_link_libraries(${TARGET_NAME} yaml-cpp)
    enable_testing()
    add_test(NAME MyTests COMMAND ${TEST_NAME})
endif()
