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

#include <boost/asio.hpp>
#include <memory>

// Forward declarations
namespace boostcraft {
    namespace network {
        class Request;
        class Response;
    }
    class interval_timer;
}


namespace boostcraft {

class Player : public network::Connection
{
public:
    explicit Player(std::unique_ptr<Connection::socket_t>); 
    ~Player();

/******************************************************************************
 * Function: disconnect
 *
 * Raises a disconnect event that will cause the server to remove this player.
 */
    void disconnect(std::string const& reason);

/******************************************************************************
 * Function: handshake
 *
 * Sets the username and initializes the Player -- call exactly once after
 * receiving a handshake request from the client.
 */
    void handshake(std::string const& username);

    void updatePosition(double x, double y, double z);
    void updateLook(float yaw, float pitch);
    void updateHealth(short health);

    int id; // entity id
    std::string name();

private:
    void dispatch(network::Request const&);

    std::shared_ptr<Player> shared_from_this();

    std::string name_;

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

    std::unique_ptr<interval_timer> timer_;

    void log(std::string message);
};

} // end namespace
