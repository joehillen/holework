// packetparser.cpp

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

#include "packetparser.h"

#include <iostream>
#include <iomanip>
#include <vector>
#include <boost/system/error_code.hpp>
#include <boost/asio.hpp>

namespace boostcraft { namespace network { 

PacketParser::PacketParser()
    : buffer_()
{
}

boost::asio::streambuf& PacketParser::buffer()
{
  return buffer_;
}

size_t PacketParser::done(const boost::system::error_code& error, 
                          size_t bytes_read)
{
    if (error) 
        return 0;

    if (buffer_.size() == 0)
        return buffer_.max_size();

    if ( !packet_ )
    {
        uint8_t id = boost::asio::buffer_cast<const uint8_t*>(buffer_.data())[0];
        buffer_.consume(1);
        packet_ = makerequest(id);
    }

    // Read whatever is available into the packet object
    if (packet_->read(buffer_) == 0)
        return 0;

    return buffer_.max_size();
}

std::unique_ptr<Request> PacketParser::consumePacket()
{
    return move(this->packet_);
}


}} //end namespace boostcraft::network

