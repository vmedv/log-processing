cmake_minimum_required(VERSION 3.21)
project(CompClubBin)

set(CMAKE_CXX_STANDARD 20)

file(GLOB SRC *.cpp *.h)
get_filename_component(main ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp ABSOLUTE)
list(REMOVE_ITEM SRC "${main}")

add_library(compclub_lib STATIC ${SRC})
add_executable(compclub ${main})
target_link_libraries(compclub compclub_lib)

