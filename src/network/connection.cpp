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

#include "log.h"

namespace xim { namespace network {

Connection::Connection(std::unique_ptr<socket_t> sock)
    : sock(std::move(sock)), buffer(), started(false)
{
}

Connection::~Connection()
{
    sock->close();
}

void Connection::start()
{
    if (started)
    {
        throw std::runtime_error("Connection already started!");
    }
    try {
        startRead();
        started = true;
    }
    catch(std::bad_weak_ptr&)
    {
        throw std::runtime_error("Connection started without existing shared_ptr!");
    }
}

void Connection::stop(std::string const& reason)
{
    sock->close();
    disconnected(reason);
}

void Connection::startRead()
{
    using namespace std::placeholders;
    boost::asio::async_read(*this->sock,
        /* buffer */
        this->buffer,
        /* completion condition */
        std::bind(&Connection::readPacket, this, _1, _2),
        /* read handler */
        std::bind(&Connection::handleRead, shared_from_this(), _1, _2));
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
    else if (sock->is_open() && error != boost::asio::error::operation_aborted)
    {
        log(INFO, "Network", "Error reading socket: " + error.message());
        disconnected("socket read error");
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
                std::bind(&Connection::handleWrite, shared_from_this(), _1, _2));
        }
    }
    else if (sock->is_open() && error != boost::asio::error::operation_aborted)
    {
        log(ERROR,"Network", "Error writing to socket: " + error.message());
        disconnected("socket write error");
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
            std::bind(&Connection::handleWrite, shared_from_this(), _1, _2));
    }
}

}} // end namespace xim

