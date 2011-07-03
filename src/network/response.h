// response.h

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

#include <boost/asio.hpp>
#include <iosfwd>

namespace boostcraft {

// Forward Declarations
class Chunk;
struct ChunkPosition;
struct EntityPosition;
struct BlockPosition;

namespace network { 

enum ResponseType
{
    RESPONSE_KEEP_ALIVE         = 0x00,
    RESPONSE_LOGIN              = 0x01,
    RESPONSE_HANDSHAKE          = 0x02,
    RESPONSE_CHAT               = 0x03,
    RESPONSE_UPDATE_TIME        = 0x04,
    RESPONSE_ENTITY_EQUIPMENT   = 0x05,
    RESPONSE_SPAWN_POS          = 0x06,
    RESPONSE_UPDATE_HEALTH      = 0x07,
    RESPONSE_POSITION_LOOK      = 0x0D,
    RESPONSE_NAMED_ENTITY_SPAWN = 0x14,
    RESPONSE_PICKUP_SPAWN       = 0x15,
    RESPONSE_COLLECT_ITEM       = 0x16,
    RESPONSE_ADD_OBJECT         = 0x17,
    RESPONSE_MOB_SPAWN          = 0x18,
    RESPONSE_ENTITY_PAINTING    = 0x19,
    RESPONSE_PRECHUNK           = 0x32,
    RESPONSE_CHUNK              = 0x33,
    // TODO: add the rest of this stuff
};

struct Response
{
    std::shared_ptr<boost::asio::streambuf> data;
    
    Response() 
        : data(new boost::asio::streambuf())
    {
    }

    boost::asio::streambuf& buffer()
    {
        return *(this->data);
    }

    void raw_string(std::string const& s);

    // TODO: default copy and assignment don't really make sense here, because
    // Response is mutable and the changes would be visible through its shared
    // pointer to the response data. Response should probably be immutable; we
    // could use a ResponseStream to create Responses; ResponseStream would be
    // non-copyable but mutable, Response would be immutable but copyable.
    // 
    Response(Response const&) = default;
    Response& operator=(Response const&) = default;
};

std::ostream& operator<<(std::ostream& os, Response const& r);

Response& operator<<(Response& os, int8_t n);
Response& operator<<(Response& os, int16_t n);
Response& operator<<(Response& os, int32_t n);
Response& operator<<(Response& os, int64_t n);
Response& operator<<(Response& os, std::string const& s);
Response& operator<<(Response& os, float n);
Response& operator<<(Response& os, double n);

/*
 * Response construction functions
 */
Response keepalive();
Response chatmessage(std::string const& msg);
Response handshake(std::string const& msg);
Response loginresponse(int32_t eid, int64_t seed, int8_t dim);
Response spawnresponse(BlockPosition pos);
Response chunkresponse(ChunkPosition pos, Chunk const& chunk);
Response positionlookresponse(EntityPosition position,
                              double stance, float yaw, float pitch,
                              bool on_ground);

}} //end namespace boostcraft::network

