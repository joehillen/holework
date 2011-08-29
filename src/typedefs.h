// typedefs.h

#pragma once

#include <memory>

namespace xim {
    class World;
    class Player;
    class Chunk;
    class ChunkPosition;

    typedef std::shared_ptr<World> world_ptr;
    typedef std::shared_ptr<Player> player_ptr;
    typedef std::shared_ptr<Chunk> chunk_ptr;
}

