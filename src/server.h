// server.h
//
#pragma once

#include "network/tcpserver.h"
#include "typedefs.h"

#include <memory>
#include <list>

namespace boostcraft
{
    // Forward declarations
    class Player;
    class World;
    namespace event {
        class LoginRequestEvent;
        class PlayerDisconnectEvent;
        class ChatEvent;
    }

    class Server : private network::TcpServer
    {
    public:
        Server(boost::asio::io_service& io, 
              boost::asio::ip::tcp::endpoint& ep);

        void addWorld(world_ptr world);

    private:
        void connect(std::unique_ptr<boost::asio::ip::tcp::socket> socket);

        /* EVENT HANDLERS */
        void onLogin(event::LoginRequestEvent& e);
        void onPlayerDisconnect(event::PlayerDisconnectEvent& e);
        void onChat(event::ChatEvent& e);

    private:
        world_ptr world;
        uint32_t entity_id;
    };

} // namespace boostcraft

