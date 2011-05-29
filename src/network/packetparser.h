// packetparser.h

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

#include <boost/system/error_code.hpp>
#include <boost/asio.hpp>
#include <memory>

#include "request.h" // std::unique_ptr requires complete type :I

namespace boostcraft { namespace network {

class Request;

/*
 * PacketParser facilitates the reading of Minecraft packets with boost::asio
 * read() or async_read(). A PacketParser's buffer and completion function can
 * be provided in the asio read call, and then the read handler will be called
 * as soon as the PacketParser has parsed one complete Minecraft packet.
 */
class PacketParser
{
public:
    PacketParser();

    /**
     * Function: buffer
     *
     * Returns a reference to the buffer used for this packet reader.
     */
    boost::asio::streambuf& buffer();

    /**
     * Function: done
     *
     * Completion condition for use with boost::asio::async_read.
     */
    size_t done(const boost::system::error_code& error, size_t bytes_read);

    /**
     * Function: consumePacket
     *
     * Returns a pointer to the packet most recently completed by this reader;
     * also nulls the internal packet pointer, preparing the reader to begin a
     * fresh packet read.
     * 
     * Call this function ONCE each time the completion condition returns zero.
     * Any other call will throw a (TODO) incomplete_request exception.
     */
    std::unique_ptr<Request> consumePacket();

private:
    boost::asio::streambuf buffer_;
    std::unique_ptr<Request> packet_;
};


}} //end namespace boostcraft::network

