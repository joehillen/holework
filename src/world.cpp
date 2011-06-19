// world.cpp
// blah blah copyright barf

#include "world.h"

#include "chunk.h"
#include "log.h"
#include "player.h"
#include "network/response.h"
#include "event/types.h"

#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

namespace boostcraft {

void World::spawnPlayer(std::shared_ptr<Player> player)
{
    log(INFO, "World", "Spawning " + player->name());
    player->deliver(network::spawnresponse(10, 10, 100));

    player->deliver(network::positionlookresponse(
        /*x,z,y,stance*/ 0, 0, 500, 501.6,
        /* yaw, pitch */ 512, 90,
        /* on_ground  */ true));

    // Add player to the world
    players.push_back(player);

    // Generate needchunk events
    // NOTE: this is very stupid
    using namespace event;
    for (int x = -8; x < 8; ++x) {
        for (int z = -8; z < 8; ++z) {
            async_fire(NeedChunkEvent(this, x, z));
        }
    }

    Chunk chunk;
    Block block { 1, 0, 15, 15 };
    for(int x = 0; x < 16; ++x) {
        for (int z = 0; z < 16; ++z) {
            chunk.set(x, z, 0, block);
        }
    }
    for(int x = -1; x < 2; ++x) {
        for (int z = -1; z < 2; ++z) {
            player->deliver(network::chunkresponse(x, z, chunk));
        }
    }
        
}


void World::newChunkHandler(event::NewChunkEvent& e)
{
    log(INFO, "World", "Got a new chunk from the magic chunk hole");
    log(INFO, "World", "Delivering it to everybody...");

    foreach(std::shared_ptr<Player> player, players)
    {
        player->deliver(network::chunkresponse(e.x, e.z, *e.chunk));
    }
}

} // namespace boostcraft

