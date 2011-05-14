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

namespace boostcraft {

/*
 * Log event handler
 */
void logHandler(LogEvent& event)
{
    std::cout << "LOGGED: " << event.source_name << " "  << event.message << "\n";
    // Now we sneakily cancel the event!
    event.canceled = true;
}


void loginHandler(LoginRequestEvent& e)
{
    e.player.id = newEntityID();
    e.player.deliver(boostcraft::network::loginresponse(e.player.id,0,0));
    log("loginhandler:", 
        boost::str(boost::format("Login request from %1%."
                    " Given entity id %2%") 
                    % e.player.username() 
                    % e.player.id));
    
    e.player.deliver(network::positionlookresponse(0, 0, 0, 0, 0, 0, 1));

    Chunk chunk;
    for(int x = -5; x < 5; ++x)
        for(int z = -5; z < 5; ++z)
            e.player.deliver(network::chunkresponse(x, z, chunk));


}

void lookHandler(PlayerLookEvent& event)
{
    std::stringstream str;
    str << event.player.username() << ": " << "Yaw: " << event.yaw << " Pitch: " << event.pitch;
    log("lookHandler", str.str());
}

void positionHandler(PlayerPositionEvent& event)
{
    std::stringstream str;
    str << event.player.username() << ": " << "X: " << event.x << " Z: " << event.z << " Y: " << event.y;
    log("lookHandler", str.str());
}

void ongroundHandler(PlayerOnGroundEvent& event)
{
    std::stringstream str;
    str << event.player.username() << ": " << "onground: " << event.on_ground;
    log("ongroundHandler", str.str());
}


} //end namespace boostcraft

