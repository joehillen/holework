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

#include <iostream>
#include <string>
#include <vector>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "packet/packetparser.h"
#include "connection.h"

Connection::Connection(boost::asio::io_service& io)
    : soc(io)
{
}


boost::asio::ip::tcp::socket& Connection::socket()
{
    return this->soc;
}


void Connection::startRead()
{
    using namespace boost::asio;
    using boost::bind;

    async_read(this->soc,
        /* buffer */
        this->parser.buffer(),
        /* completion condition */
        bind(&PacketParser::done, &this->parser,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred),
        /* read handler */
        bind(&Connection::handleRead, shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
}

void Connection::start()
{
    std::cout << "Listening for a client...\n";
    startRead();
}

/*
 * This is called when a full packet has been read, or when the connection was
 * interrupted by some error.
 */
void Connection::handleRead(const boost::system::error_code& error, size_t bytes_read)
{
    if (!error)
    {
        std::cout << "Got " << bytes_read << " bytes from the client.\n";
        Request::pointer packet = this->parser.consumePacket();

        this->dispatch(packet);

        startRead();
    }
    else
        std::cout << "THERE WAS AN ONOS\n";
}

void Connection::handleWrite(const boost::system::error_code& error, 
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
        std::cout << "ERROR WRITING! (we should probably do something)";
    }
}

void Connection::deliver(Response const& packet)
{
    std::cout << "Connection> delivering a packet!\n";
    std::cout << "Connection> " << packet << "\n";

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

