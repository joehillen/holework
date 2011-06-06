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

#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <queue>
#include "response.h"
#include "request.h"

namespace boostcraft {
namespace network {

class Connection
    : public std::enable_shared_from_this<Connection>
{
public:
    friend class TcpServer;
    typedef boost::asio::ip::tcp::socket socket_t;

    explicit Connection(std::unique_ptr<socket_t>);
    virtual ~Connection();

    /// Begin asynchronous reading on the connection.
    void start();

    /// Halt pending operations and close socket
    void stop(std::string const& reason);

    /// Send a response packet asynchronously to the connected client.
    void deliver(Response const&);

    /// Disable copying and assignment
    Connection(Connection const&) = delete; 
    Connection& operator=(Connection const&) = delete;

private:
    /// Called to process a request from the client
    virtual void dispatch(Request const&) = 0;

    /// Called when the connection is being closed
    virtual void disconnected(std::string const& reason) = 0;

    void startRead();
    void handleRead(boost::system::error_code const& error, size_t bytes_read);
    void handleWrite(boost::system::error_code const& error, size_t bytes_written);
    size_t readPacket(boost::system::error_code const& error, size_t bytes_read);

    std::unique_ptr<
        boost::asio::ip::tcp::socket> sock;

    boost::asio::streambuf buffer;
    std::unique_ptr<Request> packet;
    std::queue<Response> writeQueue;
    bool started;
};

}} //end namespace boostcraft::network

