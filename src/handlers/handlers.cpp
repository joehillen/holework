// handlers.cpp

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

#include "handlers.h"

#include <boost/format.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include "chunk.h"
#include "event/types.h"
#include "log.h"
#include "network/response.h"
#include "player.h"

namespace boostcraft {
namespace handlers {

using namespace boostcraft::event;

/*
 * Temporary solution for assigning unique EIDs
 */
int newEntityID()
{
    static int id = 0;
    return id++;
}


/*
 * Log event handler
 */
void logHandler(LogEvent& event)
{
    std::string type;
    //Dear WinDevs, fix this!
    std::string color = "";
    std::string color_end = "\033[00m";
    
    switch (event.type)
    {
        case INFO:
            type = "INFO";
            break;
        case WARN:
            type = "WARN";
            color = "\033[01;33m"; //yellow
            break;
        case ERROR:
            type = "ERROR";
            color = "\033[22;31m"; //red
            break;
        case DEBUG:
            type = "DEBUG";
            color = "\033[22;32m"; //green
            break;
        default:
            type = "LOG";
    }
    std::cout << color << type << ":" << event.source_name << ": "
        << event.message << color_end << std::endl; 
    // Now we sneakily cancel the event!
    event.canceled = true;
}

void lookHandler(PlayerLookEvent& event)
{
//    std::stringstream str;
//    str << event.player->name() << ": " << "Yaw: " << event.yaw << " Pitch: " << event.pitch;
//    log(DEBUG, "lookHandler", str.str());
}

void positionHandler(PlayerPositionEvent& event)
{
    std::stringstream str;
    str << event.player->name() << ": " << "X: " << event.x << " Z: " << event.z << " Y: " << event.y;
    log(DEBUG, "positionHandler", str.str());
}

void ongroundHandler(PlayerOnGroundEvent& event)
{
    std::stringstream str;
    str << event.player->name() << ": " << "onground: " << event.on_ground;
    log(DEBUG, "ongroundHandler", str.str());
}

void needChunkHandler(NeedChunkEvent& event)
{
    std::stringstream ss;
    ss << "Need chunk at " << event.x << ", " << event.z
       << " for world " << &event.world;
    log(INFO, "handlers", ss.str()); 
}

void init()
{
    listen(logHandler);
    listen(lookHandler);
    listen(positionHandler);
    listen(ongroundHandler);
}

}} //end namespace boostcraft::handlers

