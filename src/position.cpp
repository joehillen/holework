
#include "position.h"
#include "chunk.h"
#include <math.h>

namespace xim {

BlockPosition::BlockPosition(EntityPosition const& epos)
    : x(floor(epos.x)), z(floor(epos.z)), y(floor(epos.y))
{
}

ChunkPosition::ChunkPosition(BlockPosition const& pos) 
    : x(pos.x / Chunk::size_x), z(pos.z / Chunk::size_z)
{
}

ChunkPosition::ChunkPosition(int x, int z) 
    : x(x), z(z)
{
}

}
