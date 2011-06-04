// server.h
//
#pragma once

#include "network/tcpserver.h"

#include <memory>
#include <list>

namespace boostcraft
{
  // Forward declarations
  class Player;

  class Server : private network::TcpServer
  {
  public:
      Server(boost::asio::io_service& io, 
              boost::asio::ip::tcp::endpoint& ep);

  private:
      std::list<std::shared_ptr<Player>> players;
  };

} // namespace boostcraft

