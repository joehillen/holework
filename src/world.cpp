// world.cpp
// blah blah copyright barf

#include "world.h"

#include "chunk.h"
#include "cache.h"
#include "log.h"
#include "player.h"
#include "network/response.h"
#include "event/types.h"

#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

namespace boostcraft {

void World::sendSpawn(player_ptr player)
{
    // If player needs no more chunks and is waiting to spawn...
    if (needs_chunks.get(player).empty() && 
            spawning.count(player))
    {
        log(INFO, "World", "Spawning " + player->name());
        player->deliver(network::spawnresponse({10, 10, 100}));

        player->deliver(network::positionlookresponse(
            /* position */ {0, 0, 65},
            /* stance */ 66.6,
            /* yaw, pitch */ 512, 90,
            /* on_ground  */ true));

        spawning.erase(player);
    }
}

void World::spawnPlayer(player_ptr player)
{
    using namespace event;

    // 0. Add player to the world
    players_.insert(player);

    player->world_ = std::weak_ptr<World>(shared_from_this());

    // 0.5. Mark player waiting for spawn
    spawning.insert(player);

    // 1. Update needed list for player
    for (int x = -8; x < 8; ++x) {
        for (int z = -8; z < 8; ++z) {

            std::shared_ptr<Chunk> chunk = cache.get({x, z});

            if (chunk) 
            {
                log(DEBUG, "World", "Sending chunk from cache to " + player->name());
                player->deliver(network::chunkresponse({x, z}, *chunk));
            }
            else
            {
                async_fire(NeedChunkEvent(shared_from_this(), x, z));
                needs_chunks.add(player, {x, z});
            }
        }
    }

    sendSpawn(player);
}

void World::newChunkHandler(event::NewChunkEvent& e)
{
    log(DEBUG, "World", "Got a new chunk from the magic chunk hole");

    ChunkPosition pos { e.x, e.z };

    foreach (player_ptr player, needs_chunks.get(pos))
    {
        log(DEBUG, "World", "Sending new chunk to " + player->name());
        player->deliver(network::chunkresponse({e.x, e.z}, *e.chunk));

        // player no longer needs the chunk we just gave them
        needs_chunks.remove(player, pos);

        sendSpawn(player);
    }
}

void World::moveHandler(event::PlayerPositionEvent& e)
{
    // All players must be at y-level 100
    if (e.position.y < 100)
    {
        EntityPosition pos = {e.position.x,e.position.z,100};
        e.player->deliver(network::positionlookresponse(
            pos,
            1.6, e.player->yaw(),
            e.player->pitch(), 1));
        e.player->updatePosition(pos);
    }
}

void World::onPlayerDisconnect(event::PlayerDisconnectEvent& e)
{
    players_.erase(e.player);
}

std::set<player_ptr> World::players() const
{
    return this->players_;
}

} // namespace boostcraft

