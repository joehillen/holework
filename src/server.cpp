// server.cpp
//
#include "server.h"
#include "player.h"

#include <iostream>

namespace boostcraft {

using boost::asio::ip::tcp;

Server::Server(boost::asio::io_service& io, tcp::endpoint& endpoint)
    : TcpServer(io, endpoint)
{
} 

void Server::connect(std::unique_ptr<tcp::socket> socket)
{
    std::cout << "Creating a new player!\n";
    // Create a player from the socket
    players.push_back(
            std::shared_ptr<Player>(
                new Player(std::move(socket))));
}

} // namespace boostcraft

