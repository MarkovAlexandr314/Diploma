cmake_minimum_required(VERSION 3.16)
project(CGNS_App C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_path(CGNS_INCLUDE_DIR NAMES cgnslib.h
    PATHS /usr/local/include/cgns /usr/include/cgns)
find_library(CGNS_LIBRARY NAMES cgns
    PATHS /usr/local/lib /usr/lib)

if(NOT CGNS_INCLUDE_DIR OR NOT CGNS_LIBRARY)
    message(FATAL_ERROR "Не найден cgnslib.h или libcgns")
endif()

find_package(Python 3.12 REQUIRED COMPONENTS Interpreter Development)
find_package(HDF5 REQUIRED COMPONENTS C)
find_package(pybind11 REQUIRED)
include(CTest)   # определяет BUILD_TESTING (=ON) и вызывает enable_testing()

if(BUILD_TESTING)
    find_package(GTest REQUIRED)
endif()

add_subdirectory(src)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
