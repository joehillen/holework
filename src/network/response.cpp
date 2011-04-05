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

#include <iostream>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/zlib.hpp>

#include "../chunk.h" // TODO: this is kinda messy; maybe responses that depend
                      // on classes outside the network namespace should be
                      // promoted out of the network namespace, so that we only
                      // have downward dependencies

uint64_t htonll(uint64_t n)
{
    return (((uint64_t)htonl(n)) << 32) | (htonl(n >> 32));
}


namespace boostcraft { namespace network { 

void Response::raw_string(std::string const& s)
{
    std::ostream out(this->data.get());
    out << s;
}

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


Response chunkresponse(uint32_t x, uint32_t z, Chunk const& chunk)
{
    Response r;

    // Always include a pre-chunk "packet" as a header
    r << (uint8_t)RESPONSE_PRECHUNK << x << z << (uint8_t) 1; //1 for initialize mode
    // Send chunk
    // TODO: refactor this coordinate stuff
    r << (uint8_t)RESPONSE_CHUNK;
    r << (uint32_t)(x * 16);
    r << (uint16_t)0;
    r << (uint32_t)(z * 16);
    r << (uint8_t)Chunk::size_x;
    r << (uint8_t)Chunk::size_y;
    r << (uint8_t)Chunk::size_z;

    // Compressify!
    std::stringstream source_data;
    std::stringstream compressed_data;

    source_data << chunk;

    boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
    in.push(boost::iostreams::zlib_compressor());
    in.push(source_data);
    boost::iostreams::copy(in, compressed_data);

    std::string s = compressed_data.str();

    r << (uint32_t)s.size();
    r.raw_string(s);

    return r;
}

}} //end namespace boostcraft::network

