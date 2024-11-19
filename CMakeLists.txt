cmake_minimum_required(VERSION 3.6)	
set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")
project(myDriverFTDI)
# include_directories(C:/Windows/System32)
include_directories(ftSrc)

add_executable(main_file main.c)

set_target_properties(main_file PROPERTIES RUNTIME_OUTPUT_DIRECTORY ..)
# add_subdirectory(ftSrc)
link_directories(ftSrc)
target_link_libraries(main_file C:/Users/dmitr/Desktop/ft2232_driver/ftd2xx64.dll)

