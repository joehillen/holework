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
    
    size_t need = 0;

    while (need == 0)
    {
        need = fieldList.front()->readFrom(buffer_);

        if (need == 0)
        {
            fieldList.pop_front();
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

