cmake_minimum_required(VERSION 3.22.0)
set(CMAKE_CXX_STANDARD 20)

project(nons)

# main part of project

option(BUILD_PROJECT "Build project" ON)
option(PROFILING "Add profiling flag" OFF)

if (PROFILING)
    message(STATUS "Profiling enabled")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
    SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg")
endif()


file(GLOB_RECURSE src "source/*.cpp")
list(REMOVE_ITEM src "${CMAKE_CURRENT_SOURCE_DIR}/source/main.cpp")
file(GLOB_RECURSE tests "test/*.cpp")

add_subdirectory(${CMAKE_SOURCE_DIR}/external/yaml-cpp/)
add_subdirectory(${CMAKE_SOURCE_DIR}/external/matplotplusplus/)
add_subdirectory(${CMAKE_SOURCE_DIR}/external/cppcoro/)

include_directories(
    ${CMAKE_SOURCE_DIR}/source/
    ${CMAKE_SOURCE_DIR}/external/spdlog/include/
    ${CMAKE_SOURCE_DIR}/external/cxxopts/include/
    ${CMAKE_SOURCE_DIR}/external/cppcoro/include/
)

if (BUILD_PROJECT)
    set(TARGET_NAME "${PROJECT_NAME}")
    add_executable(${TARGET_NAME} ${src} source/main.cpp)
    target_link_libraries(${TARGET_NAME} PRIVATE yaml-cpp matplot cppcoro)
    # if (DEFINED SCHEDULER)
    target_compile_definitions(${TARGET_NAME} PRIVATE SCHEDULER=ASYNC_SCHEDULER)
    # endif()
    target_compile_options(${TARGET_NAME} PRIVATE -Wall -Werror -Wextra)
endif()

# tests
option(BUILD_TESTS "Build tests" ON)

if(BUILD_TESTS)
    add_subdirectory(external/googletest)
    set(TEST_NAME "test_nons")
    add_executable(${TEST_NAME} ${src} ${tests})
    target_include_directories(${TEST_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/external/yaml-cpp/include)
    target_compile_options(${TEST_NAME} PRIVATE -Wall -Wextra)
    target_include_directories(${TEST_NAME} PUBLIC "source")
    target_link_libraries(${TEST_NAME} gtest gtest_main yaml-cpp matplot cppcoro)
    enable_testing()
    add_test(NAME MyTests COMMAND ${TEST_NAME})
endif()
