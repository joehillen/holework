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
#include "position.h"
#include "uniconv.h"
#include "inetconv.h"

#include <iostream>
#include <iomanip>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/zlib.hpp>

#include "chunk.h" 

namespace boostcraft {
namespace network { 

std::ostream& operator<<(std::ostream& os, Response const& r)
{
    const uint8_t* buf = boost::asio::buffer_cast<const uint8_t*>(r.data->data());

    for(size_t i = 0; i < r.data->size(); ++i) {
        os << std::hex << (int)buf[i] << " ";
    }

    return os;
}

void Response::raw_string(std::string const& s)
{
    std::ostream out(this->data.get());
    out << s;
}

Response& operator<<(Response& os, int8_t n)
{
    std::ostream out(os.data.get());
    out.put(n);
    return os;
}

Response& operator<<(Response& os, int16_t n)
{
    std::ostream out(os.data.get());
    n = ntoh(n);
    out.write((const char*)&n, 2);
    return os;
}

Response& operator<<(Response& os, int32_t n)
{
    std::ostream out(os.data.get());
    n = ntoh(n);
    out.write((const char*)&n, 4);
    return os;
}

Response& operator<<(Response& os, int64_t n)
{
    std::ostream out(os.data.get());
    n = ntoh(n);
    out.write((const char*)&n, 8);
    return os;
}

Response& operator<<(Response& os, std::string const& s)
{
    std::ostream out(os.data.get());
    uint16_t len = ntoh((uint16_t)s.length());
    out.write((const char*)&len, 2);
    out << s;
    return os;
}

Response& operator<<(Response& os, std::u16string const& s)
{
    std::ostream out(os.data.get());
    uint16_t len = ntoh((uint16_t)s.length());
    out.write((const char*)&len, 2);
    out.write((const char*)s.c_str(), s.length()*sizeof(char16_t));
    return os;
}

Response& operator<<(Response& os, float n)
{
    std::ostream out(os.data.get());
    n = ntoh(n);
    out.write((const char*)&n, 4);
    return os;
}

Response& operator<<(Response& os, double n)
{
    std::ostream out(os.data.get());
    n = ntoh(n);
    out.write((const char*)&n, 8);
    return os;
}

Response keepalive()
{
    Response r;
    r << (int8_t)RESPONSE_KEEP_ALIVE;
    return r;
}

Response chatmessage(std::string const& msg)
{
    Response r;
    r << (int8_t)RESPONSE_CHAT;
    r << utf8toucs2(msg);
    return r;
}

Response handshake(std::string const& hash)
{
    Response r;
    r << (int8_t)RESPONSE_HANDSHAKE;
    r << utf8toucs2(hash);
    return r;
}

Response loginresponse(int32_t entityId, int64_t seed, int8_t dimension)
{
    Response r;
    r << (int8_t)RESPONSE_LOGIN;
    r << entityId;
    r << utf8toucs2("NOT USED");
    r << seed;
    r << dimension;
    return r;
}


Response chunkresponse(ChunkPosition pos, Chunk const& chunk)
{
    Response r;

    // Always include a pre-chunk "packet" as a header
    r << (int8_t)RESPONSE_PRECHUNK << pos.x << pos.z << (int8_t) 1; //1 for initialize mode
    // Send chunk
    // TODO: refactor this coordinate stuff
    r << (int8_t)RESPONSE_CHUNK;
    r << (int32_t)(pos.x * 16);
    r << (int16_t)0;
    r << (int32_t)(pos.z * 16);
    r << (int8_t)Chunk::size_x;
    r << (int8_t)Chunk::size_y;
    r << (int8_t)Chunk::size_z;

    // Compressify!
    std::stringstream source_data;
    std::stringstream compressed_data;

    source_data << chunk;

    boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
    in.push(boost::iostreams::zlib_compressor());
    in.push(source_data);
    boost::iostreams::copy(in, compressed_data);

    std::string s = compressed_data.str();

    r << (int32_t)s.size();
    r.raw_string(s);

    return r;
}

Response spawnresponse(BlockPosition pos)
{
    Response r;
    r << (int8_t)RESPONSE_SPAWN_POS;
    r << pos.x << pos.y << pos.z;
    return r;
}

Response positionlookresponse(EntityPosition pos,
                              double stance, float yaw, float pitch,
                              bool on_ground)
{
    Response r;

    r << (int8_t)RESPONSE_POSITION_LOOK;
    r << pos.x << pos.y << pos.y + stance << pos.z;
    r << yaw << pitch;
    r << (int8_t)on_ground;

    return r;
}

}} //end namespace boostcraft::network

