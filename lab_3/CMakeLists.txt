cmake_minimum_required(VERSION 3.12)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project("Lab3" CXX)

# Find MPI
find_package(MPI REQUIRED)

# Include MPI headers
include_directories(${MPI_CXX_INCLUDE_PATH})

add_executable("Lab3"
"function.cpp"
"function.h"
"main.cpp"
"matrix.h"
"matrix.cpp"
"globals.h"
"globals.cpp"
)

target_link_libraries(Lab3 ${MPI_CXX_LIBRARIES})

if(MSVC)
    target_include_directories(Lab3 PRIVATE ${MPI_CXX_INCLUDE_PATH})
    target_compile_definitions(Lab3 PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

if(WIN32 AND MSVC)
    add_custom_command(TARGET Lab3 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "========================================"
        COMMAND ${CMAKE_COMMAND} -E echo "Make sure msmpi.dll is in your PATH or"
        COMMAND ${CMAKE_COMMAND} -E echo "copy it from: C:/Program Files (x86)/Microsoft SDKs/MPI/Bin/x64/"
        COMMAND ${CMAKE_COMMAND} -E echo "to the output directory: $<TARGET_FILE_DIR:Lab3>"
        COMMAND ${CMAKE_COMMAND} -E echo "========================================"
        COMMENT "MPI DLL reminder"
    )
endif()