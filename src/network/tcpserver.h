// tcpserver.h

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
#include <functional>
#include <memory>

#include "../log.h"

namespace boostcraft { namespace network {

using boost::asio::ip::tcp;

class TcpServer
{
public:
    TcpServer(boost::asio::io_service& io, tcp::endpoint& endpoint)
        : io_(io), acceptor_(io, endpoint)
    {
        startAccept();
    }

    virtual ~TcpServer()
    {
    }

private:
    void startAccept()
    {
        log(INFO, "TcpServer", "Waiting for a connection...");

        // Create a socket
        tcp::socket* sock = new tcp::socket(io_);

        //
        // WARNING! Hackery ahead!
        // For some reason, std::bind does not support move semantics; so we
        // cannot create a std::unique_ptr here and transfer ownership to the
        // AcceptHandler through the std::bind.
        //
        // Instead we create a naked pointer and trust that AcceptHandler will
        // do the right thing and wrap it in a std::unique_ptr.
        //
        acceptor_.async_accept(
            *sock,
            std::bind(&TcpServer::handleAccept,
                this,
                sock,
                std::placeholders::_1));
    }

    void handleAccept(tcp::socket* sock,
            const boost::system::error_code& error)
    {
        if (!error)
        {
            sock->remote_endpoint().address();
            log(INFO, "TcpServer", "Got a connection!");

            // Hand off the newly accepted socket connection with a unique_ptr
            // so the implementer of connect() takes ownership
            connect(std::unique_ptr<tcp::socket>(sock));

            // Begin waiting for next connection
            startAccept();
        }
        else
        {
            delete sock;
            throw boost::system::system_error(error);
        }
    }

    virtual void connect(std::unique_ptr<tcp::socket> sock) = 0;

private:
    boost::asio::io_service& io_;
    tcp::acceptor acceptor_;
};

}} //end namespace boostcraft::network
