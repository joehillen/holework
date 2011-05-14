// events.cpp

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

#include "events.h"

namespace boostcraft {

boost::asio::io_service the_service;

boost::asio::io_service& io_service()
{
    return the_service;
}

boost::signals2::signal<void(LoginRequestEvent&)> LoginRequestEvent::signal;
boost::signals2::signal<void(LogEvent&)> LogEvent::signal;
boost::signals2::signal<void(PlayerNeedsChunkEvent&)> PlayerNeedsChunkEvent::signal;
boost::signals2::signal<void(PlayerLookEvent&)> PlayerLookEvent::signal;
boost::signals2::signal<void(PlayerPositionEvent&)> PlayerPositionEvent::signal;
boost::signals2::signal<void(PlayerOnGroundEvent&)> PlayerOnGroundEvent::signal;

}

