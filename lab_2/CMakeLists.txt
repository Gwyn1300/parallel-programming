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

if(MINGW)
    target_compile_options(Lab1 PRIVATE -fopenmp)
    target_link_options(Lab1 PRIVATE -fopenmp)
    target_link_libraries(Lab1 PRIVATE gomp)
    target_compile_definitions(Lab1 PRIVATE _OPENMP)
    message(STATUS "OpenMP manually enabled for MinGW")
else()
    find_package(OpenMP)
    if(OpenMP_CXX_FOUND)
        target_link_libraries(Lab1 PUBLIC OpenMP::OpenMP_CXX)
    endif()
endif()
