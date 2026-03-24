cmake_minimum_required(VERSION 3.12)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


project("Lab1" CXX)

add_executable("Lab1"
"function.cpp"
"function.h"
"main.cpp"
"matrix.h"
"matrix.cpp"
"verification.py"
"dataProcessing.py"
"globals.h"
"globals.cpp"
)

