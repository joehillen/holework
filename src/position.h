// position.h
#pragma once

#include <sys/types.h>

namespace xim {

struct EntityPosition
{
    double x;
    double z;
    double y;

    EntityPosition() : x(), z(), y() { }
    EntityPosition(double x, double z, double y)
        : x(x), z(z), y(y) { }
    EntityPosition(EntityPosition const& other) = default;
    EntityPosition& operator=(EntityPosition const& rhs) = default;
};

struct BlockPosition
{
    int32_t x;
    int32_t z;
    int32_t y;

    BlockPosition() : x(), z(), y() { }
    explicit BlockPosition(EntityPosition const&);
    BlockPosition(int32_t x, int32_t z, int32_t y)
        : x(x), z(z), y(y) { }
    BlockPosition(BlockPosition const& other) = default;
    BlockPosition& operator=(BlockPosition const& rhs) = default;
};

/// Represents the position of a chunk in the world
struct ChunkPosition
{
    int x;
    int z;

    ChunkPosition(ChunkPosition const&) = default;
    ChunkPosition(int x, int z);
    explicit ChunkPosition(BlockPosition const&);
    ChunkPosition& operator=(ChunkPosition const&) = default;

    bool operator==(ChunkPosition const& rhs) const {
        return x == rhs.x && z == rhs.z;
    }

    // This is used for map
    bool operator<(ChunkPosition const& rhs) const 
    {
        if (rhs.x == this->x) {
            return rhs.z < this->z;
        } 
        return rhs.x < this->x;
    }
};


} // end namespace xim

