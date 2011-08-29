#include "python.h"

#include <iostream>

namespace xim {
namespace python {

namespace py = boost::python;

py::object main_module; 
py::object main_namespace;

void init()
{
    Py_InitializeEx(0);
    main_module = py::import("__main__");
    main_namespace = main_module.attr("__dict__");


    try
    {
        py::object val = exec("1 + 3");
        int result = py::extract<int>(val);
        std::cout << "The value of 4 is: " << result << "\n";
    }
    catch (py::error_already_set& e)
    {
        PyErr_Print();
    }
}

py::object exec(std::string const& code)
{
    try {
        return py::eval(code.c_str(), main_namespace);
    }
    catch (py::error_already_set& e)
    {
        PyErr_Print();
        return py::object();
    }
}

}} // namespace xim::python

