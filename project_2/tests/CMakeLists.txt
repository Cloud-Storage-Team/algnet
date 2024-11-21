# tests/CMakeLists.txt


find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(runTests number_manager_test.cpp ../src/number_manager.cpp)

target_link_libraries(runTests ${GTEST_LIBRARIES} pthread)
target_include_directories(runTests PUBLIC ../include)

enable_testing()

add_test(NAME MyTests COMMAND runTests)