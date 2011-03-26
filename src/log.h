// log.h

#pragma once

#include "events.h"
#include <string>

namespace boostcraft {

static void log(std::string source, std::string message)
{
    LogEvent logevent(message);
    logevent.source_name = source;
    LogEvent::signal(logevent);
}

} //end namespace
