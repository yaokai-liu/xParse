cmake_minimum_required(VERSION 3.25)
project(xParse:test C)

set(CMAKE_C_STANDARD 23)

add_executable(test-execute vm/test-execute.c)
target_link_libraries(test-execute PRIVATE xParse-vm check)

add_executable(test-inst vm/test-inst.c)
target_link_libraries(test-inst PRIVATE xParse-vm)

add_executable(non-test main.c)
target_link_libraries(non-test PRIVATE xParse-vm)
