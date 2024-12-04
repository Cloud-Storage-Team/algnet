cmake_minimum_required(VERSION 3.16)

project(power_tcp)

set(CMAKE_CXX_STANDARD 20)

enable_testing()

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_library(power_tcp_lib src/PowerTCPCongestionWindowHandler.cpp include/CongestionWindowHandler.h include/PowerTCPCongestionWindowHandler.h)
target_include_directories(power_tcp_lib PRIVATE include)

add_executable(power_tcp src/main.cpp)
target_include_directories(power_tcp PRIVATE include)
target_link_libraries(power_tcp power_tcp_lib)

add_executable(power_tcp_test tests/PowerTCPCongestionWindowHandler.cpp)
target_include_directories(power_tcp_test PRIVATE include)
target_link_libraries(power_tcp_test power_tcp_lib GTest::GTest GTest::Main)

add_test(NAME PowerTCPTest COMMAND power_tcp_test)