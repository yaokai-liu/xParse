cmake_minimum_required(VERSION 3.25)
project(xParse:test C)

set(CMAKE_C_STANDARD 23)

add_executable(test vm/test-execute.c)
target_link_libraries(test PRIVATE check xParse-vm)

add_executable(non-test main.c)
target_link_libraries(non-test PRIVATE xParse-vm)
