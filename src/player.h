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
#include "player.h"
#include "position.h"

#include <boost/asio.hpp>
#include <memory>

// Forward declarations
namespace boostcraft {
    namespace network {
        class Request;
        class Response;
    }
    namespace event {
        class interval_timer;
    }
}

namespace boostcraft {

class Player : public network::Connection
{
public:
    explicit Player(uint32_t id, std::unique_ptr<Connection::socket_t>); 
    ~Player();

    // Set username and initialize keepalive in response to a client handshake
    void handshake(std::string const& username);

    uint32_t id();
    // Return player nickname
    std::string name();

    void updatePosition(EntityPosition position);
    void updateLook(float yaw, float pitch);
    void updateHealth(short health);

private:
    /// Initiates dispatch for a client request
    void dispatch(network::Request const&);

    /// Responds to disconnection by raising event
    void disconnected(std::string const& reason);

    /// Conversions for base class's enable_shared_from_this
    std::shared_ptr<Player> shared_from_this();
    std::shared_ptr<Player const> shared_from_this() const;

    uint32_t id_;
    std::string name_;

    /* Position */
    EntityPosition position_;
    EntityPosition last_position_;
    BlockPosition spawn_;

    /* Look */
    float yaw_;
    float pitch_;

    /* State */
    bool on_ground_;
    bool digging_;
    short health_;

    std::unique_ptr<event::interval_timer> timer_;

    void log(std::string message);
};

typedef std::shared_ptr<Player> player_ptr;
typedef std::list<player_ptr> player_list;

} // end namespace
