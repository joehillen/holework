// events.cpp

#include "events.h"

namespace boostcraft {

boost::signals2::signal<void(LogEvent&)> LogEvent::signal;
boost::signals2::signal<void(LoginRequestEvent&)> LoginRequestEvent::signal;

}

