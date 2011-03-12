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

#include "packetparser.h"
#include "connection.h"

class Connection
    : public boost::enable_shared_from_this<Connection>
{
public:
    typedef boost::shared_ptr<Connection> pointer;

    Connection(boost::asio::io_service& io)
        : socket_(io)
    {
    }

    static pointer create(boost::asio::io_service& io)
    {
        return pointer(new Connection(io));
    }

    tcp::socket& socket()
    {
        return socket_;
    }

    void startRead()
    {
        using namespace boost::asio;
        using boost::bind;

        async_read(socket_,
            /* buffer */
            parser.buffer(),
            /* completion condition */
            bind(&PacketParser::done, &parser,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred),
            /* read handler */
            bind(&Connection::handleRead, shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
    }

    void start()
    {
        std::cout << "Listening for a client...\n";
        startRead();
    }

    void handleRead(const boost::system::error_code& error, size_t bytes_read)
    {
        if (!error)
        {
            std::cout << "Got " << bytes_read << " bytes from the client.\n";
            Packet::pointer packet = parser.consumePacket();
            packet->connection = shared_from_this();
            startRead();
        }
        else
            std::cout << "THERE WAS AN ONOS\n";
    }

private:
    tcp::socket socket_;
    PacketParser parser;
};
