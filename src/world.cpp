// world.cpp
// blah blah copyright barf

#include "world.h"

#include "chunk.h"
#include "cache.h"
#include "log.h"
#include "player.h"
#include "network/response.h"
#include "event/types.h"

#include <sstream>
#include <cmath>
#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

namespace xim {

World::World(unsigned int max) 
    : cache(max)
{
    position_timer_.reset(new event::interval_timer(50, 
        std::bind(&World::checkPositions, this))
    );
}

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

            chunk_ptr chunk = cache.get({x, z});

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

void World::onPlayerDisconnect(event::PlayerDisconnectEvent& e)
{
    players_.erase(e.player);
}

std::set<player_ptr> World::players() const
{
    return this->players_;
}

void World::checkPositions()
{

    foreach(player_ptr player_p, players_)
    {
        Player& player = *player_p;

        double dx = player.position_.x - player.last_position_.x;
        double dz = player.position_.z - player.last_position_.z;
        double dy = player.position_.y - player.last_position_.y;

        double delta = std::sqrt(dx*dx + dz*dz + dy*dy);

        if (delta > 0.5d)
        {
            log(INFO, "World", player.name() + " moved illegally!");
        }

        player.last_position_ = player.position_;
    }
}

} // namespace xim

