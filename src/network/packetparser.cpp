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
#include "packet.h"

#include <iostream>
#include <iomanip>
#include <vector>
#include <boost/system/error_code.hpp>
#include <boost/asio.hpp>

namespace boostcraft { namespace network { 

PacketParser::PacketParser()
    // Hard-coded buffer size for now.
    // NOTE: buffer size must be _at least_ 8 bytes to accomodate delayed reading
    // of all primitive types.
    : buffer_(512)
{
}

boost::asio::streambuf& PacketParser::buffer()
{
  return buffer_;
}

size_t PacketParser::done(const boost::system::error_code& error, 
                          size_t bytes_read)
{
    if (error) {
        return 0;
    }

    if (buffer_.size() == 0)
        return buffer_.max_size();


    if ( !packet )
    {
        uint8_t id = boost::asio::buffer_cast<const uint8_t*>(buffer_.data())[0];
        buffer_.consume(1);

        std::pair<Request::pointer, std::list<PacketField::pointer> > packet_pair = packetFactory(id);
        
        packet = packet_pair.first;
        fieldList = packet_pair.second;
    }

    if (fieldList.empty())
        return 0;
    
    // Read fields until we empty the buffer
    while(buffer_.size() > 0)
    {
        int need = fieldList.front()->readFrom(buffer_);

        // When a field is complete, remove it from the list
        if (need == 0)
        {
            fieldList.pop_front();

            // Done with packet when there are no more fields
            if (fieldList.empty())
                return 0;
        }
    }

    return buffer_.max_size();
}

Request::pointer PacketParser::consumePacket()
{
   Request::pointer this_packet(this->packet);
   this->packet.reset();
   return this_packet;
}


}} //end namespace boostcraft::network

