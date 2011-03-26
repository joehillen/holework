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
#include "network/packet/packet.h"
#include "network/connection.h"
#include "log.h"

#define MAX_DELTA 10000 //change this or make it configurable?

namespace boostcraft
{

Player::Player(boost::asio::io_service& io)
    : Connection(io)
{
    
}

void Player::log(std::string message)
{
    boostcraft::log("Player: " + this->username_, message);
}

void Player::deliver(network::Response const& packet)
{
    Connection::deliver(packet);
}

void Player::dispatch(network::Request::pointer packet)
{
    using namespace network;
    Request* p = packet.get();

    switch(packet->type)
    {
    case REQUEST_KEEP_ALIVE:
        this->deliver(keepalive());
        break;
    case REQUEST_HANDSHAKE:
    {
        log("Got Handshake request.");
        if (username_.empty())
        {
            this->username_ = ((HandshakeRequest*)p)->username;
            log("Handshake request.");
            this->deliver(handshake("-"));
        }
        else
        {
            log("ERROR: Username is already set.");
        }
        break;
    }
    case REQUEST_LOGIN:
        std::cout << "Responding to login\n";
        this->deliver(loginresponse(1337, 0, 0));
        break;
    default:
        std::cout << "Unhandled packet!\n";
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

} // end namespace
