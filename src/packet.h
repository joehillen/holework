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

enum ResponseType
{
    RESPONSE_KEEP_ALIVE         = 0,
    RESPONSE_LOGIN              = 1,
    RESPONSE_HANDSHAKE          = 2,
    RESPONSE_CHAT               = 3,
    RESPONSE_UPDATE_TIME        = 4,
    RESPONSE_ENTITY_EQUIPMENT   = 5,
    RESPONSE_SPAWN_POS          = 6,
    RESPONSE_UPDATE_HEALTH      = 7,
    RESPONSE_POSITION_LOOK      = 0xD,
    RESPONSE_NAMED_ENTITY_SPAWN = 0x14,
    RESPONSE_PICKUP_SPAWN       = 0x15,
    RESPONSE_COLLECT_ITEM       = 0x16,
    RESPONSE_ADD_OBJECT         = 0x17,
    RESPONSE_MOB_SPAWN          = 0x18,
    RESPONSE_ENTITY_PAINTING    = 0x19,
    // TODO: add the rest of this stuff
};

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
    Packet(uint8_t id) : type(id) { }
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



struct Response
{
    uint8_t id;
    Response(uint8_t id) : id(id) { }
};

struct KeepAliveResponse : public Response
{
    KeepAliveResponse() : Response(RESPONSE_KEEP_ALIVE) { }
};

struct HandshakeResponse : public Response
{
    HandshakeResponse(std::string const& hash) 
        : Response(RESPONSE_HANDSHAKE), connectionHash(hash) { }

    std::string connectionHash;
};

struct ChatReponse : public Response
{
    ChatResponse(std::string const& msg) 
        : Response(RESPONSE_CHAT), message(msg) { }
    std::string message;
};













std::pair<Packet::pointer, std::list<PacketField::pointer> > packetFactory(int id);

