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
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include "connection.h"
#include "../player.h" // TODO: this is icky
#include "../log.h"

namespace boostcraft {


namespace network {


using boost::asio::ip::tcp;

class TcpServer
{
public:
    TcpServer(boost::asio::io_service& io, tcp::endpoint& endpoint)
        : io_(io), acceptor_(io, endpoint)
    {
        startAccept();
    }

    void startAccept()
    {
        log(INFO, "TcpServer","Waiting for a connection...");

        Connection::pointer new_connection(
                                (Connection*) new Player(io_));

        acceptor_.async_accept(new_connection->socket(),
            boost::bind(&TcpServer::handleAccept, this, 
                new_connection,
                boost::asio::placeholders::error));
    }

    void handleAccept(Connection::pointer connection, 
                        const boost::system::error_code& error)
    {
        if (!error)
        {
            log(INFO, "TcpServer","Got a connection!");
            connection->start();
            startAccept();
        }
        else
            throw boost::system::system_error(error);
    }

private:
    boost::asio::io_service& io_;
    tcp::acceptor acceptor_;
};

}} //end namespace boostcraft::network
