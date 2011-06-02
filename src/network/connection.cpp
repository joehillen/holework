// connection.cpp

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

#include "connection.h"

#include <iostream>
#include <string>
#include <vector>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "../log.h"

namespace boostcraft { namespace network {

Connection::Connection(boost::asio::io_service& io)
    : soc(io), buffer()
{
}


boost::asio::ip::tcp::socket& Connection::socket()
{
    return this->soc;
}

void Connection::start()
{
    log(INFO, "Network", "Listening for a client...");
    startRead();
}

void Connection::startRead()
{
    using namespace boost::asio;
    using namespace boost::system;
    using boost::bind;

    async_read(this->soc,
        /* buffer */
        this->buffer,
        /* completion condition */
        bind(&Connection::readPacket, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred),
        /* read handler */
        bind(&Connection::handleRead, shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
}

/*
 * Called when data is received from the client; incrementally reads into the
 * current Request and returns 0 when a complete request has been received.
 */
size_t Connection::readPacket(boost::system::error_code const& error,
        size_t bytes_read)
{
    if (error)
        return 0;

    if (buffer.size() == 0)
        return buffer.max_size();

    // Create new Request for each packet signature byte
    if (!packet)
    {
        uint8_t id = boost::asio::buffer_cast<const uint8_t*>(buffer.data())[0];
        buffer.consume(1);
        packet = makerequest(id);
    }

    if (packet->read(buffer) == 0)
        return 0;

    return buffer.max_size();
}

/*
 * This is called when a full packet has been read, or when the connection was
 * interrupted by some error.
 */
void Connection::handleRead(boost::system::error_code const& error,
        size_t bytes_read)
{
    if (!error)
    {
        // Dispatch and then destroy the packet we just read
        dispatch(*packet);
        packet.reset();
        startRead();
    }
    else
        log(INFO, "Network", "Error reading socket: connection closed.");
}

void Connection::handleWrite(boost::system::error_code const& error, 
        size_t bytes_written)
{
    writeQueue.pop();
    if (!error)
    {
        if (!writeQueue.empty())
        {
            async_write(this->soc,
                writeQueue.front().buffer(),
                bind(&Connection::handleWrite, shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
        }
    }
    else
    {
        log(ERROR,"Network", "Error writing to socket"
                "(TODO: we should probably do something)");
    }
}

void Connection::deliver(Response const& packet)
{
    bool empty = writeQueue.empty();
    writeQueue.push(packet);
    if (empty)
    {
        async_write(this->soc,
            writeQueue.front().buffer(),
            bind(&Connection::handleWrite, shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
    }
}

}} // end namespace boostcraft

