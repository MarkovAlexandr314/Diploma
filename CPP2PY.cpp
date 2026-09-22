// pywrap.cpp
#include <pybind11/pybind11.h>
// #include <pybind11/eigen.h>
#include "Parser.h"
#include <pybind11/stl.h>
namespace py = pybind11;
constexpr auto byref = py::return_value_policy::reference_internal;

PYBIND11_MODULE(MyLib, m)
{
    m.doc() = "optional module docstring";

    py::class_<Parser>(m, "Parser")
        .def(py::init<const std::string &>())
        .def("ReadSolution", &Parser::ReadSolution, "read_solution")
        .def("GetFlowSolution", &Parser::GetFlowSolution,
             py::call_guard<py::gil_scoped_release>(),
             "GetFlowSolution");
}