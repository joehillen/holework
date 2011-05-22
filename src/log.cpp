// log.cpp

/***********************************************************************
* Copyright (C) 2011 Holework Project
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Affero General Public License as
* published by the Free Software Foundation, either version 3 of the
* License, or (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Affero General Public License for more details.
* 
* You should have received a copy of the GNU Affero General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*************************************************************************/

#include "log.h"
#include "events.h"
#include <string>

namespace boostcraft {

void log(LogType type, std::string source, std::string message)
{
    if (debug_mode == true || type != DEBUG)
    {
        LogEvent logevent(type, message);
        logevent.source_name = source;
        async_fire(logevent);
    }
}

} //end namespace
