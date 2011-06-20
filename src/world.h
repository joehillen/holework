// world.h

#pragma once

#include "event/event.h"
#include "event/types.h"
#include "multimap.h"
#include "chunk.h"
#include "player.h"
#include <functional>
#include <set>

namespace boostcraft {

class World
{
public:
    template<typename G>
    World(G generator)
    {
        using namespace boostcraft::event;
        using std::placeholders::_1;
        listen_world<NeedChunkEvent>(generator, this);
        listen_world<NewChunkEvent>(
            std::bind(&World::newChunkHandler, this, _1),
            this);
    }

    void spawnPlayer(std::shared_ptr<Player> p);
    void addPlayer(std::shared_ptr<Player> p);
    void rmPlayer(std::shared_ptr<Player> p);

private:
    
    std::list<std::shared_ptr<Player>> players;
    //ChunkCache cache;

    // Who needs what chunks
    Multimap<player_ptr, ChunkPosition> needs_chunks;

    // Who is waiting to spawn?
    std::set<player_ptr> spawning;

    /// Event handlers
    void newChunkHandler(event::NewChunkEvent& e); 
    void movePlayer(event::PlayerPositionEvent& e);
};

} // end namespace boostcraft
