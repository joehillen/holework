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

#include "../log.h"

namespace boostcraft { namespace network {

Connection::Connection(std::unique_ptr<socket_t> sock)
    : sock(std::move(sock)), buffer()
{
    startRead();
}

Connection::~Connection()
{
}

void Connection::start()
{
    log(INFO, "Network", "Listening for a client...");
    startRead();
}

void Connection::startRead()
{
    using namespace boost::asio;
    using namespace std::placeholders;

    async_read(*this->sock,
        /* buffer */
        this->buffer,
        /* completion condition */
        std::bind(&Connection::readPacket, this, _1, _2),
        /* read handler */
        std::bind(&Connection::handleRead, this, _1, _2));
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
    else if (error != boost::asio::error::operation_aborted)
    {
        log(INFO, "Network", "Error reading socket: connection closed.");
        disconnect();
    }
}

void Connection::handleWrite(boost::system::error_code const& error, 
        size_t bytes_written)
{
    using namespace std::placeholders;

    // Clear the write that just completed
    writeQueue.pop();
    if (!error)
    {
        // Continue with any pending write
        if (!writeQueue.empty())
        {
            async_write(*this->sock,
                writeQueue.front().buffer(),
                /* write completion handler */
                std::bind(&Connection::handleWrite, this, _1, _2));
        }
    }
    else if (error != boost::asio::error::operation_aborted)
    {
        log(ERROR,"Network", "Error writing to socket"
                "(TODO: we should probably do something)");
        disconnect();
    }
}

void Connection::deliver(Response const& packet)
{
    using namespace std::placeholders;

    bool empty = writeQueue.empty();
    writeQueue.push(packet);

    // Start a new write if there were no pending writes
    if (empty)
    {
        async_write(*this->sock,
            writeQueue.front().buffer(),
            /* write completion handler */
            std::bind(&Connection::handleWrite, this, _1, _2));
    }
}

}} // end namespace boostcraft

