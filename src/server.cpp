// server.cpp
//
#include "server.h"

namespace boostcraft {

using boost::asio::ip::tcp;

Server::Server(boost::asio::io_service& io, tcp::endpoint& endpoint)
    : TcpServer(io, endpoint)
{
}

} // namespace boostcraft

