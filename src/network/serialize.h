// serialize.h

#pragma once

#include <boost/asio.hpp>

class Response;

boost::asio::streambuf serialize(Response const& packet);
