// handlers.h

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

#pragma once

#include <sstream>
#include <string>
#include <iostream>
#include "network/response.h"
#include "boost/format.hpp"

#include "events.h"
#include "player.h"
#include "log.h"
#include "chunk.h"

namespace boostcraft {

static int newEntityID()
{
    static int id = 0;
    return id++;
}

void logHandler(LogEvent& event);
void loginHandler(LoginRequestEvent& e);
void lookHandler(PlayerLookEvent& event);
void positionHandler(PlayerPositionEvent& event);
void ongroundHandler(PlayerOnGroundEvent& event);

} //end namespace
