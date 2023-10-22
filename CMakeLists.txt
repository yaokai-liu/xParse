cmake_minimum_required(VERSION 3.25)
project(xParse C)

set(CMAKE_C_STANDARD 23)
#set(CMAKE_BUILD_TYPE Release)
#add_compile_options("-O3")
add_compile_options("-nostdinc")
add_compile_options("-nostdlib")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output)

add_compile_definitions("XTYPES_USING_STDC_TYPE")
add_compile_definitions("XDEF_USING_C_STD_DEF")
add_compile_definitions("XPARSE_CHAR_USING_ASCII")
add_compile_definitions("XPARSE_USING_STD_FORMAT")
add_compile_definitions("XPARSE_DEBUG")

include_directories(xdef)
include_directories(include)
include_directories(mem)

aux_source_directory(mem MEM)
add_library(mem ${MEM})
link_libraries(mem)

aux_source_directory(string STRING)
add_library(string ${STRING})
link_libraries(string)

aux_source_directory(builtin BUILTIN)

file(GLOB_RECURSE GRAMMAR grammar/*.c grammar/*.h)
add_library(grammar ${BUILTIN} ${GRAMMAR})
target_include_directories(grammar PRIVATE string vm)

aux_source_directory(vm VM)
add_library(vm ${VM})
target_include_directories(vm PRIVATE string)

add_subdirectory(test)
