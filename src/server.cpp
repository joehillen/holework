// server.cpp
//
#include "server.h"
#include "player.h"
#include "events.h"

#include <iostream>

namespace boostcraft {

using boost::asio::ip::tcp;

Server::Server(boost::asio::io_service& io, tcp::endpoint& endpoint)
    : TcpServer(io, endpoint)
{
    // Register event handlers
    //listen(std::bind(&Server::onPlayerDisconnect, this));

    boost::function<void(PlayerDisconnectEvent&)>
        p = [=](PlayerDisconnectEvent& e)
    {
        players.remove(e.player);
    };

    listen(p);

    //
    // TODO: "listen" needs to return a signal connection so we can remove the
    //       listener when this object is destroyed... Granted, it's less of an
    //       issue with Server, since there is one Server instance that doesn't
    //       go out of scope until the io_service has already ceased running,
    //       but other use cases may require it.
    //
}

void Server::connect(std::unique_ptr<tcp::socket> socket)
{
    std::cout << "Creating a new player!\n";
    // Create a player from the socket
    players.push_back(
            std::shared_ptr<Player>(
                new Player(std::move(socket))));
}

void Server::onPlayerDisconnect(PlayerDisconnectEvent& e)
{
    players.remove(e.player);
}

} // namespace boostcraft

