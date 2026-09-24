#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "Parser.h"

namespace py = pybind11;

PYBIND11_MODULE(CGNS_App, m)
{
    py::class_<Parser>(m, "Parser")
        .def(py::init<const std::string &>())
        .def("ReadSolution", &Parser::ReadSolution, "read_solution")
        .def("GetFlowSolution", &Parser::GetFlowSolution,
             py::call_guard<py::gil_scoped_release>(),
             "GetFlowSolution")
        .def("ReadCoordinates", &Parser::ReadCoordinates, "ReadCoordinates")
        .def("GetCoordinates", &Parser::GetCoordinates,
             py::call_guard<py::gil_scoped_release>(),
             "GetCoordinates")
        .def("GetFlowSolMap", &Parser::GetFlowSolMap, "GetFlowSolMap")
        .def("GetZoneNameMap", &Parser::GetZoneNameMap, "GetZoneNameMap");
}