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


void sendSpawnStuff(Player& player)
{
    player.deliver(network::spawnresponse(10, 10, 100));

    // HACK: send inventory
    network::Response r;
    r << (uint8_t)0x68;
    r << (uint8_t)0;
    r << (uint16_t)45;

    for(int i = 0; i < 45; ++i) {
        r << (uint16_t)331;
        r << (uint8_t)2;
        r << (uint16_t)0;
    }
    player.deliver(r);

    player.deliver(network::positionlookresponse(0, 0, 200, 201.6, 512, 90, true));
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

    Chunk chunk;
    for(int x = -8; x < 8; ++x)
        for(int z = -8; z < 8; ++z)
            e.player.deliver(network::chunkresponse(x, z, chunk));

    boostcraft::schedule(3000, boost::bind(sendSpawnStuff, boost::ref(e.player)));
}


void lookHandler(PlayerLookEvent& event)
{
//    std::stringstream str;
//    str << event.player.username() << ": " << "Yaw: " << event.yaw << " Pitch: " << event.pitch;
//    log("lookHandler", str.str());
}

void positionHandler(PlayerPositionEvent& event)
{
//    std::stringstream str;
//    str << event.player.username() << ": " << "X: " << event.x << " Z: " << event.z << " Y: " << event.y;
//    log("lookHandler", str.str());
}

void ongroundHandler(PlayerOnGroundEvent& event)
{
    std::cout << "ONGROUND HANDLER\n";
    std::cout << event.player.username() << "\n";
//    std::stringstream str;
//    str << event.player.username() << ": " << "onground: " << event.on_ground;
//    log("ongroundHandler", str.str());
}


} //end namespace boostcraft

