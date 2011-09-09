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
#include <gtest/gtest.h>

namespace xim {

World::World(unsigned int max) 
    : cache(max)
{
    position_timer_.reset(new event::interval_timer(1000, 
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
            /* position */ {0, 0, 66},
            /* stance */ 1.6,
            /* yaw, pitch */ 512, 90,
            /* on_ground  */ true));

        spawning.erase(player);
        player->spawned_ = true;
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
                //log(DEBUG, "World", "Sending chunk from cache to " + player->name());
                player->deliver(network::chunkresponse({x, z}, *chunk));
            }
            else
            {
                async_fire(NeedChunkEvent(shared_from_this(), x, z));
                needs_chunks.add(player, {x, z});
            }
        }
    }

    player->last_position_ = {0, 0, 66};
    sendSpawn(player);
}

void World::newChunkHandler(event::NewChunkEvent& e)
{
    ChunkPosition pos { e.x, e.z };

    foreach (player_ptr player, needs_chunks.get(pos))
    {
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

/**
 * Perodic check of all the player positions
 */
void World::checkPositions()
{

    foreach(player_ptr player_p, players_)
    {
        Player& player = *player_p;

        if (!player.spawned_) 
        {
            continue;
        }

        EntityPosition pos = player.position_;

        std::stringstream ss;
        ss << player.name() << " pos = x: " << pos.x 
            << " z: " << pos.z
            << " y: " << pos.y;
        log(DEBUG, "World::checkPosition", ss.str());

        double dx = pos.x - player.last_position_.x;
        double dz = pos.z - player.last_position_.z;
        double dy = pos.y - player.last_position_.y;

        double delta = std::sqrt(dx*dx + dz*dz + dy*dy);

        if (delta > 0.5d)
        {
            log(ERROR, "World::checkPosition", player.name() + " moved illegally!");
        }

        
        BlockPosition block_pos(pos);
        Block b;
        try 
        {
            b = this->getBlock(block_pos);
        }
        catch (std::exception& e)
        {
            std::cerr << "World::checkPosition: " << e.what() << std::endl;
            continue;
        }


        if (b.type != 0)
        {
            std::stringstream ss;
            ss << player.name() << " collided with terrain at " << pos;
            log(ERROR, "World::checkPosition", ss.str());

            player.updatePosition(player.last_position_);
            player.deliver(network::positionlookresponse(
                      player.last_position_ + EntityPosition(0,0,2),
                      1.6,
                      player.yaw_,
                      player.pitch_,
                      player.on_ground_));
        }
        else
        {
            player.last_position_ = pos;
        }
    }
}

Block World::getBlock(BlockPosition const& pos)
{
    ChunkPosition chunk_pos(pos);
    chunk_ptr chunk = this->cache.get(chunk_pos);

//    if (!chunk)
//    {
//        chunk = store_(chunk_pos);
//    }
//    if (!chunk)
//    {
//        chunk = generator_(chunk_pos);
//    }

    /* Convert a block position to a position inside a chunk*/
    auto algo = [](int p, int size)
    {
        if (p < 0)
        {
            p = (p % size) + size;
        }
        else
        {
            p = p % size;
        }
        return p;
    };

    int x = algo(pos.x, Chunk::size_x);
    int z = algo(pos.z, Chunk::size_z);
    int y = pos.y;

    return chunk->get(x, z, y);
}


TEST(WorldTests, GetBlock)
{
    World w(1);
    chunk_ptr c(new Chunk());
    Block block_a { 12, 4, 9, 12 };
    Block block_b { 11, 9, 0, 10 };
    Block block_c { 1, 0, 6, 15 };
    c->set(0, 1, 2, block_a);
    c->set(6, 11, 99, block_b);
    c->set(4, 9, 127, block_c);
    w.cache.add({0,0}, c);
    ASSERT_EQ(block_a, w.getBlock({0, 1, 2}));
    ASSERT_EQ(block_b, w.getBlock({6, 11, 99}));
    ASSERT_EQ(block_c, w.getBlock({4, 9, 127}));
}

TEST(WorldTests, GetBlockNegative)
{
    World w(1);
    chunk_ptr c(new Chunk());
    Block block_a { 12, 4, 9, 12 };
    Block block_b { 11, 9, 0, 10 };
    Block block_c { 1, 0, 6, 15 };
    c->set(15, 14, 2, block_a);
    c->set(6, 11, 99, block_b);
    c->set(4, 9, 127, block_c);
    w.cache.add({-1,-1}, c);
    ASSERT_EQ(block_a, w.getBlock({-1, -2, 2}));
    ASSERT_EQ(block_b, w.getBlock({-10, -5, 99}));
    ASSERT_EQ(block_c, w.getBlock({-12, -7, 127}));
}

} // namespace xim
