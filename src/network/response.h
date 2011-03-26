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
#include <boost/shared_ptr.hpp>
#include <iomanip>
#include <ostream>


namespace boostcraft { namespace network { 

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

struct Response
{
    boost::shared_ptr<boost::asio::streambuf> data;
    
    Response() 
        : data(new boost::asio::streambuf(64))
    {
    }

    boost::asio::streambuf& buffer()
    {
        return *(this->data);
    }
};

static std::ostream& operator<<(std::ostream& os, Response const& r)
{
    const uint8_t* buf = boost::asio::buffer_cast<const uint8_t*>(r.data->data());

    for(size_t i = 0; i < r.data->size(); ++i) {
        os << std::hex << (int)buf[i] << " ";
    }

    return os;
}

Response& operator<<(Response& os, uint8_t n);
Response& operator<<(Response& os, uint16_t n);
Response& operator<<(Response& os, uint32_t n);
Response& operator<<(Response& os, uint64_t n);
Response& operator<<(Response& os, std::string const& s);

/*
 * Response construction functions
 */
Response keepalive();
Response chatmessage(std::string const& msg);
Response handshake(std::string const& msg);
Response loginresponse(uint32_t eid, uint64_t seed, uint8_t dim);

}} //end namespace boostcraft::network

