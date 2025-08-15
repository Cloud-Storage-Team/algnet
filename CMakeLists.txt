cmake_minimum_required(VERSION 3.22.0)
set(CMAKE_CXX_STANDARD 23)

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

include_directories(
    ${CMAKE_SOURCE_DIR}/source/
    ${CMAKE_SOURCE_DIR}/external/spdlog/include/
    ${CMAKE_SOURCE_DIR}/external/cxxopts/include/
)

add_subdirectory(${CMAKE_SOURCE_DIR}/external/yaml-cpp/)
add_subdirectory(${CMAKE_SOURCE_DIR}/external/matplotplusplus/)

if (BUILD_PROJECT)
    set(TARGET_NAME "${PROJECT_NAME}")
    add_executable(${TARGET_NAME} ${src} source/main.cpp)
    target_link_libraries(${TARGET_NAME} yaml-cpp matplot)
    target_compile_options(${TARGET_NAME} PRIVATE -Wall -Werror -Wextra)

    if (DEFINED LOG_LEVEL)
        target_compile_definitions(${TARGET_NAME} PRIVATE LOG_LEVEL=${LOG_LEVEL})
    endif()

    target_include_directories(${TARGET_NAME} PUBLIC "source")
endif()

# tests
option(BUILD_TESTS "Build tests" ON)

if(BUILD_TESTS)
    add_subdirectory(external/googletest)
    set(TEST_NAME "test_nons")
    add_executable(${TEST_NAME} ${src} ${tests})
    
    target_include_directories(${TEST_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/external/yaml-cpp/include)
    target_include_directories(${TEST_NAME} PUBLIC "source")

    target_compile_options(${TEST_NAME} PRIVATE -Wall -Wextra)
    target_link_libraries(${TEST_NAME} gtest gtest_main yaml-cpp matplot)

    if (DEFINED LOG_LEVEL)
        target_compile_definitions(${TEST_NAME} PRIVATE LOG_LEVEL=${LOG_LEVEL})
    endif()

    enable_testing()
    add_test(NAME MyTests COMMAND ${TEST_NAME})
endif()
