cmake_minimum_required(VERSION 3.25)
project(xParse C)

set(CMAKE_C_STANDARD 23)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output)

add_compile_definitions("XTYPES_USING_STDC_TYPE")
add_compile_definitions("XDEF_USING_C_STD_DEF")
add_compile_definitions("XPARSE_CHAR_USING_ASCII")
add_compile_definitions("XPARSE_USING_STD_FORMAT")
#add_compile_definitions("XPARSE_DEBUG")

include_directories(xdef)
include_directories(mem)
include_directories(string)
include_directories(vm)
include_directories(builtin)
include_directories(grammar)
include_directories(xparse)

aux_source_directory(mem MEM)
aux_source_directory(string STRING)
aux_source_directory(vm VM)
file(GLOB_RECURSE GRAMMAR grammar/*.c)
file(GLOB_RECURSE VM vm/*.c)
aux_source_directory(builtin BUILTIN)
aux_source_directory(codegen CODEGEN)
aux_source_directory(xparse XPARSE)

add_library(xparse ${XPARSE} ${MEM} ${STRING} ${BUILTIN} ${GRAMMAR} ${VM} ${CODEGEN})
target_compile_options(xparse PRIVATE "-nostdinc" "-nostdlib")

add_subdirectory(test)
