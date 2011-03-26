// packet.h

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

#include "packetfield.h"

#define MAX_USERNAME_LENGTH 16
namespace boostcraft { namespace network { 

enum RequestType
{
    REQUEST_KEEP_ALIVE          = 0,
    REQUEST_LOGIN               = 1,
    REQUEST_HANDSHAKE           = 2,
    REQUEST_CHAT                = 3,
    // TODO: add the rest
};



// Base for all request packets
struct Request
{
    typedef boost::shared_ptr<Request> pointer;

    uint8_t type;
    Request(RequestType id) : type(id) { }
};

struct KeepAliveRequest : public Request
{
    KeepAliveRequest() : Request(REQUEST_KEEP_ALIVE) { }
};

struct HandshakeRequest : public Request
{
    HandshakeRequest()
        : Request(REQUEST_HANDSHAKE) { }

    std::string username;
};

struct LoginRequest : public Request
{
    LoginRequest()
        : Request(REQUEST_LOGIN) { }

    uint32_t protocolVersion;
    std::string username;
    std::string password;
    uint64_t mapSeed;
    uint8_t dimension;
};

struct ChatRequest : public Request
{
    ChatRequest() : Request(REQUEST_CHAT) { }

    std::string message;
};

std::pair<Request::pointer, std::list<PacketField::pointer> > packetFactory(int id);

}} //end namespace boostcraft::network
