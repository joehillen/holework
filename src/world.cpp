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
    using namespace event;

    // 0. Add player to the world
    players.push_back(player);

    // 0.5. Mark player waiting for spawn
    spawning.insert(player);

    // 1. Update needed list for player
    for (int x = -8; x < 8; ++x) {
        for (int z = -8; z < 8; ++z) {
            //if chunk in cache:
                //player.deliver(chunk)
            //else:
                async_fire(NeedChunkEvent(this, x, z));
                needs_chunks.add(player, {x, z});
        }
    }
}


void World::newChunkHandler(event::NewChunkEvent& e)
{
    log(DEBUG, "World", "Got a new chunk from the magic chunk hole");

    ChunkPosition pos { e.x, e.z };

    foreach (player_ptr player, needs_chunks.get(pos))
    {
        player->deliver(network::chunkresponse(e.x, e.z, *e.chunk));

        // player no longer needs the chunk we just gave them
        needs_chunks.remove(player, pos);

        // If player needs no more chunks and is waiting to spawn...
        if (needs_chunks.get(player).empty() && 
                spawning.count(player))
        {
            log(INFO, "World", "Spawning " + player->name());
            player->deliver(network::spawnresponse(10, 10, 100));

            player->deliver(network::positionlookresponse(
                /*x,z,y,stance*/ 0, 0, 65, 66.6,
                /* yaw, pitch */ 512, 90,
                /* on_ground  */ true));

            spawning.erase(player);
        }
    }
}

} // namespace boostcraft

