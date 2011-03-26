// player.h

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

#include "network/connection.h"
#include "network/packet.h"
#include "network/response.h"

#include <boost/asio.hpp>

namespace boostcraft
{


class Player : private network::Connection,
               public boost::enable_shared_from_this<Player>
{
public:
    Player(boost::asio::io_service& io); 

    void deliver(network::Response const& packet);

    void updatePosition(double x, double y, double z);
    void updateLook(float yaw, float pitch);
    void updateHealth(short health);

    int id; // entity id
    std::string username();

private:
    /**
     * Called from network layer to dispatch a packet received
     * from the attached client.
     */
    void dispatch(network::Request::pointer packet);

    std::string username_;


    /* Position */
    double x_;
    double z_;
    double y_;

    /* Look */
    float yaw_;
    float pitch_;

    /* State */
    bool on_ground_;
    bool digging_;
    short health_;

    /* Spawn */
    double spawn_x_;
    double spawn_z_;
    double spawn_y_;

    void log(std::string message);
};

} // end namespace
