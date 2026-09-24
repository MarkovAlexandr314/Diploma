add_library(cgns_core STATIC
    Parser.cpp
)
set_target_properties(cgns_core PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)
target_include_directories(cgns_core PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CGNS_INCLUDE_DIR}
)

target_link_libraries(cgns_core PUBLIC
    ${CGNS_LIBRARY}
    HDF5::HDF5
)

pybind11_add_module(${PROJECT_NAME}
    PyBind_Interface.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE cgns_core)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    VERSION_INFO=${EXAMPLE_VERSION_INFO}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/scripts"
)

add_executable(cgns_app
    main.cpp
)

target_link_libraries(cgns_app PRIVATE cgns_core)