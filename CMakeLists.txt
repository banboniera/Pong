cmake_minimum_required(VERSION 3.7)
project(semestralka)

set(CMAKE_CXX_STANDARD 14)

add_executable(semestralka main.cpp)

target_link_libraries(semestralka pthread)