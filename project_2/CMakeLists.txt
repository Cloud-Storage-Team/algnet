cmake_minimum_required(VERSION 3.26)
project(express_pass)

set(CMAKE_CXX_STANDARD 20)

option(ENABLE_EXPORTS "Enable exports for the executable" ON)

# Enable profiling with gprof
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")

file(GLOB SOURCES src/*.cpp)

add_executable(express_pass ${SOURCES})

add_library(express_pass_lib SHARED ${SOURCES})

target_include_directories(express_pass PUBLIC include)
target_include_directories(express_pass_lib PUBLIC include)

# --- Testing Setup ---
add_subdirectory(googletest)

enable_testing()

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

file(GLOB TEST_SOURCES tests/*.cpp)

add_executable(express_pass_tests ${TEST_SOURCES})

target_link_libraries(express_pass_tests ${GTEST_LIBRARIES} pthread express_pass_lib)

target_include_directories(express_pass_tests PRIVATE include_tests)

add_test(NAME express_pass_tests COMMAND express_pass_tests)
