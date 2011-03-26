// tcpserver.h

#pragma once

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include "connection.h"
#include "../player.h" // TODO: this is icky

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
        std::cout << "Waiting for a connection...\n";

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
            std::cout << "Got a connection!\n";
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
