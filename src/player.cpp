// player.cpp 

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

#include "player.h"

#include "event/types.h"
#include "log.h"
#include "network/connection.h"
#include "network/request.h"
#include "network/response.h"
#include "position.h"
#include "world.h"

#include <sstream>

using namespace xim::event;

namespace xim
{

void Player::log(std::string message)
{
    xim::log(INFO, "Player: " + name(), message);
}


Player::Player(uint32_t id, std::unique_ptr<Connection::socket_t> sock)
    : Connection(std::move(sock)), id_(id), 
      timer_(0)
{
}

Player::~Player()
{
}

player_ptr Player::shared_from_this() 
{
    return std::static_pointer_cast<Player>(
            Connection::shared_from_this());
}

std::shared_ptr<Player const> Player::shared_from_this() const
{
    return std::static_pointer_cast<Player const>(
            Connection::shared_from_this());
}

uint32_t Player::id()
{
    return id_;
}

void Player::disconnected(std::string const& reason)
{
    PlayerDisconnectEvent e(shared_from_this(), reason);
    fire(e);
    log("disconnected: " + reason);
}

void Player::dispatch(network::Request const& packet)
{
    packet.dispatch(shared_from_this());
}

void Player::handshake(std::string const& username)
{
    if (name_.empty())
    {
        name_ = username;
        deliver(network::handshake("-"));
        log("Handshake request for user " + name_);

        // Initialize the keepalive timer
        timer_.reset(new interval_timer(5000,
            [this]() {
                this->deliver(network::keepalive());
            }));
    }
    else
    {
        log("Received handshake " + username + " when username already set to " + name_);
    }
}

EntityPosition Player::position()
{
    return position_;
}

void Player::updatePosition(EntityPosition pos)
{
    position_ = pos;
}

void Player::updateLook(float yaw, float pitch)
{
    yaw_ = yaw;
    pitch_ = pitch;
    /* TODO: Something that tells everyone the player look changed. */
}

void Player::updateHealth(short health)
{
    if (health >= 20)
    {
        health_ = 20;
    }
    else if (health <= 0)
    {
        health = 0;
    }
    else
    {
        health_ = health;
    }
}

std::string Player::name()
{
    return this->name_;
}

std::weak_ptr<World> Player::world() const
{
    return this->world_;    
}

float Player::yaw() const
{
    return this->yaw_;
}

float Player::pitch() const
{
    return this->pitch_;
}

} // end namespace xim
