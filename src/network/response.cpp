// response.cpp

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

#include "response.h"
#include <ostream>
#include <netinet/in.h>

uint64_t htonll(uint64_t n)
{
    return (((uint64_t)htonl(n)) << 32) | (htonl(n >> 32));
}


namespace boostcraft { namespace network { 

Response& operator<<(Response& os, uint8_t n)
{
    std::ostream out(os.data.get());
    out.put(n);
    return os;
}

Response& operator<<(Response& os, uint16_t n)
{
    std::ostream out(os.data.get());
    n = htons(n);
    out.write((const char*)&n, 2);
    return os;
}

Response& operator<<(Response& os, uint32_t n)
{
    std::ostream out(os.data.get());
    n = htonl(n);
    out.write((const char*)&n, 4);
    return os;
}

Response& operator<<(Response& os, uint64_t n)
{
    std::ostream out(os.data.get());
    n = htonll(n);
    out.write((const char*)&n, 8);
    return os;
}

Response& operator<<(Response& os, std::string const& s)
{
    std::ostream out(os.data.get());
    uint16_t len = htons(s.length());
    out.write((const char*)&len, 2);
    out << s;
    return os;
}

Response keepalive()
{
    Response r;
    r << (uint8_t)RESPONSE_KEEP_ALIVE;
    return r;
}

Response chatmessage(std::string const& msg)
{
    Response r;
    r << (uint8_t)RESPONSE_CHAT;
    r << msg;
    return r;
}

Response handshake(std::string const& hash)
{
    Response r;
    r << (uint8_t)RESPONSE_HANDSHAKE;
    r << hash;
    return r;
}

Response loginresponse(uint32_t entityId, uint64_t seed, uint8_t dimension)
{
    Response r;
    r << (uint8_t)RESPONSE_LOGIN;
    r << entityId;
    r << "THESE ARE NOT";
    r << "CURRENTLY USED";
    r << seed;
    r << dimension;
    return r;
}

}} //end namespace boostcraft::network

