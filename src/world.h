// world.h

#pragma once

#include "event/event.h"
#include "event/types.h"
#include <functional>

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


    void addPlayer(std::shared_ptr<Player> p);
    void rmPlayer(std::shared_ptr<Player> p);


private:
    std::list<std::shared_ptr<Player>> players;
    //ChunkCache cache;
    void newChunkHandler(event::NewChunkEvent& e) { }
};

} // end namespace boostcraft
