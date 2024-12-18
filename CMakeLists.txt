cmake_minimum_required(VERSION 3.26)
project(network_simulator)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCE_FILES
    src/Flow.cpp
    src/Packet.cpp
    src/Event.cpp
    src/NetworkSimulator.cpp
    src/NetworkDevice.cpp
    src/Sender.cpp
    src/Switch.cpp
    src/Receiver.cpp
    src/EventScheduler.cpp
)

add_executable(network_simulator src/main.cpp ${SOURCE_FILES})

target_include_directories(network_simulator PUBLIC include)

# Testing framework
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(network_simulator_test test/NetworkSimulatorTest.cpp ${SOURCE_FILES})
target_link_libraries(
        network_simulator_test
        GTest::gtest_main
)
target_include_directories(network_simulator_test PUBLIC include)

include(GoogleTest)
gtest_discover_tests(network_simulator_test)