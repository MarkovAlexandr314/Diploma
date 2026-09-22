cmake_minimum_required(VERSION 3.12)
project(CGNS_App C CXX)

set(CMAKE_CXX_STANDARD 17)

# Добавляем путь к подпапке /usr/local/include/cgns
find_path(CGNS_INCLUDE_DIR NAMES cgnslib.h PATHS /usr/local/include/cgns /usr/include/cgns)
find_library(CGNS_LIBRARY NAMES cgns PATHS /usr/local/lib /usr/lib)
find_package(HDF5 REQUIRED COMPONENTS C)

####### PyBind 
find_package(pybind11 REQUIRED)
pybind11_add_module(${PROJECT_NAME} CPP2PY.cpp)

target_compile_definitions(${PROJECT_NAME} PRIVATE VERSION_INFO=${EXAMPLE_VERSION_INFO})
target_include_directories(${PROJECT_NAME} PRIVATE ${PYBIND11_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE ${CGNS_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE ${CGNS_LIBRARY} HDF5::HDF5)

# if (NOT CGNS_INCLUDE_DIR OR NOT CGNS_LIBRARY)
#     message(FATAL_ERROR "Не удалось найти заголовок cgnslib.h или библиотеку libcgns")
# endif()

# message(STATUS "Найден CGNS include: ${CGNS_INCLUDE_DIR}")
# message(STATUS "Найден CGNS library: ${CGNS_LIBRARY}")

# add_executable(my_cgns_app main.cpp Parser.cpp)

# target_include_directories(my_cgns_app PRIVATE ${CGNS_INCLUDE_DIR})
# target_link_libraries(my_cgns_app PRIVATE ${CGNS_LIBRARY} HDF5::HDF5)