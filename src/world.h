// world.h

#pragma once

#include "event/event.h"

namespace boostcraft {

class World
{
public:
    template<typename G>
    World(G generator)
    {
        using namespace boostcraft::event;
        listen_world<NeedChunkEvent>(generator, this);
    }

private:
    //ChunkCache cache;
};

} // end namespace boostcraft
