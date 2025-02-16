cmake_minimum_required(VERSION 3.22.0)
set(CMAKE_CXX_STANDARD 20)

project(simulator)

# main part of project
option(BUILD_MAIN "Build project" ON)

if (BUILD_MAIN)
    set(TARGET_NAME "${PROJECT_NAME}")
    file(GLOB_RECURSE src "source/*.cpp")
    add_executable(${TARGET_NAME} main.cpp ${src})
endif()

# tests
option(BUILD_TESTS "Build tests" ON)

if(BUILD_TESTS)
    add_subdirectory(googletest)
    set(TEST_NAME "test_simulator")
    add_executable(${TEST_NAME} test/simple_test.cpp) 
    target_link_libraries(${TEST_NAME} gtest gtest_main)
    enable_testing()
    add_test(NAME MyTests COMMAND ${TEST_NAME})
endif()
