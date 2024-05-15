cmake_minimum_required(VERSION 3.21)
project(Tests)

set(CMAKE_CXX_STANDARD 20)

find_package(Catch2 REQUIRED)

file(GLOB TEST_SRC *Test.cpp)
add_executable(tests ${TEST_SRC})
target_link_libraries(tests Catch2::Catch2WithMain compclub_lib)

include(CTest)
include(Catch)
catch_discover_tests(tests)
