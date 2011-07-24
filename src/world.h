// world.h

#pragma once

#include "event/event.h"
#include "event/types.h"
#include "multimap.h"
#include "chunk.h"
#include "cache.h"
#include "player.h"
#include <functional>
#include <set>
#include <memory>

namespace boostcraft {

class World : public std::enable_shared_from_this<World>
{
public:
    World(unsigned int max) : cache(max)
    { }

    /* TODO: Don't let init() be called more than once */
    template<typename G>
    void init(G generator)
    {
        using namespace boostcraft::event;
        using std::placeholders::_1;
        listen_world<NeedChunkEvent>(generator, shared_from_this());
        listen_world<NewChunkEvent>(
            std::bind(&World::newChunkHandler, this, _1),
            shared_from_this()
        );
        listen_world<NewChunkEvent>(
            std::bind(&ChunkCache::handler, &cache, _1),
            shared_from_this()
        );
        listen_world<PlayerPositionEvent>(
            std::bind(&World::moveHandler, this, _1),
            shared_from_this()
        );
    }

    void spawnPlayer(player_ptr p);
    
    std::set<player_ptr> players() const;

private:
    // Player list 
    std::set<player_ptr> players_;

    ChunkCache cache;

    // Who needs what chunks
    Multimap<player_ptr, ChunkPosition> needs_chunks;

    // Who is waiting to spawn?
    std::set<player_ptr> spawning;

    // Send spawn to player when they are ready.
    void sendSpawn(player_ptr player);

    /// Event handlers
    void newChunkHandler(event::NewChunkEvent& e); 
    void moveHandler(event::PlayerPositionEvent& e);
    void onPlayerDisconnect(event::PlayerDisconnectEvent& e);
};

} // end namespace boostcraft
