# tests/CMakeLists.txt


find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})


add_executable(runTests number_manager_test.cpp)

target_link_libraries(runTests ${GTEST_LIBRARIES} pthread)

enable_testing()

 add_test(NAME MyTests COMMAND runTests)