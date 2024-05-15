cmake_minimum_required(VERSION 3.21)
project(CompClub)

set(CMAKE_CXX_STANDARD 20)

find_package(Catch2 REQUIRED)

enable_testing()

include_directories(include)

add_subdirectory(src)
add_subdirectory(test)
