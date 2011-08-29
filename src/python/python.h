#pragma once

#include <boost/python.hpp>
#include <boost/python/detail/wrap_python.hpp>
#include <string>

namespace xim {
namespace python {

    void init();

    boost::python::object eval(std::string const& code);

}} // namespace xim::python

