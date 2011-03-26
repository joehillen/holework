// signals.h

#pragma once

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/signals2/signal.hpp>
#include <iostream>

#include "events.h"

namespace boostcraft
{

class Signals
{
public:
    boost::signals2::signal< void (LogEvent& e) > log;
    boost::signals2::signal< void (LoginRequestEvent& e) > loginRequest;
};


} // end namespace
