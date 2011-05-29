// connection.h

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

#include <iostream>
#include <string>
#include <vector>
#include <queue>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "packetparser.h"
#include "response.h"

namespace boostcraft { namespace network {

class Request;

class Connection
    : public boost::enable_shared_from_this<Connection>
{
public:
    typedef boost::shared_ptr<Connection> pointer;

    friend class TcpServer;

    // TODO: maybe make private?
    Connection(boost::asio::io_service& io);

    /**
     * Returns the socket associated with this connection.
     */
    boost::asio::ip::tcp::socket& socket();

    /**
     * Begins asynchronous reading of packets from the connected client.
     */
    void start();

    /**
     * Delivers a packet to the connected client.
     */
    void deliver(Response const&);


private:
    void startRead();
    void handleRead(const boost::system::error_code& error, size_t bytes_read);
    void handleWrite(const boost::system::error_code& error, size_t bytes_written);

    virtual void dispatch(Request const&) = 0;

    boost::asio::ip::tcp::socket soc;
    PacketParser parser;
    std::queue<Response> writeQueue;
};

}} //end namespace boostcraft::network
