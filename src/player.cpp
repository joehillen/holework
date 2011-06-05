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

#include "events.h"
#include "log.h"
#include "network/connection.h"
#include "network/request.h"
#include "network/response.h"

#include <sstream>

namespace boostcraft
{

Player::Player(std::unique_ptr<Connection::socket_t> sock)
    : Connection(std::move(sock)),
      timer_(0)
{
}

Player::~Player()
{
    std::cout << "Destroying player " << name() << "\n";
}

void Player::log(std::string message)
{
    boostcraft::log(INFO, "Player: " + name(), message);
}

void Player::disconnect()
{
    log("disconnected.");
}

void Player::deliver(network::Response const& packet)
{
    Connection::deliver(packet);
}

void Player::dispatch(network::Request const& packet)
{
    packet.dispatch(*this);
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

void Player::updatePosition(double x, double y, double z)
{
    x_ = x;
    x_ = y;
    x_ = z;
    /* TODO: Something that tells everyone the player position changed. */
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

} // end namespace boostcraft
