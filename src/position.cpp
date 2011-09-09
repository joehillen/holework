
#include "position.h"
#include "chunk.h"
#include <math.h>
#include <ostream>
#include <gtest/gtest.h>

namespace xim {

BlockPosition::BlockPosition(EntityPosition const& epos)
    : x(floor(epos.x)), z(floor(epos.z)), y(floor(epos.y))
{
}

std::ostream& operator<<(std::ostream& os, xim::BlockPosition const& pos)
{
    os << "BlockPosition(x=" << pos.x
        << ", z=" << pos.z
        << ", y=" << pos.y
        << ")";
    return os;
}

ChunkPosition::ChunkPosition(BlockPosition const& pos) 
{
    x = pos.x / Chunk::size_x;
    if (pos.x < 0)
    {
        x -= 1;
    }

    z = pos.z / Chunk::size_z;
    if (pos.z < 0)
    {
        z -= 1;
    }
}

ChunkPosition::ChunkPosition(int x, int z) 
    : x(x), z(z)
{
}

std::ostream& operator<<(std::ostream& os, xim::ChunkPosition const& pos)
{
    os << "ChunkPosition(x=" << pos.x
        << ", z=" << pos.z
        << ")";
    return os;
}

TEST(PositionTests, BlockPosition)
{
    EntityPosition ep_a(0, 0, 0);
    BlockPosition  bp_a(ep_a);
    ASSERT_EQ(bp_a, BlockPosition(0, 0, 0));
}

TEST(PositionTests, ChunkPosZero)
{
    BlockPosition bp(4, 10, 12);
    ChunkPosition cp(bp);
    ChunkPosition cp_r(0,0);
    ASSERT_EQ(cp_r, cp);
}

TEST(PositionTests, ChunkPosOne)
{
    BlockPosition bp(16, 20, 2);
    ChunkPosition cp(bp);
    ChunkPosition cp_r(1,1);
    ASSERT_EQ(cp_r, cp);
}

TEST(PositionTests, ChunkPosRand)
{
    BlockPosition bp(33, 120, 7);
    ChunkPosition cp(bp);
    ChunkPosition cp_r(2,7);
    ASSERT_EQ(cp_r, cp);
}

TEST(PositionTests, ChunkPosNeg)
{
    BlockPosition bp(-1, -12, 103);
    ChunkPosition cp(bp);
    ChunkPosition cp_r(-1,-1);
    ASSERT_EQ(cp_r, cp);
}

TEST(PositionTests, ChunkPosNegTwo)
{
    BlockPosition bp(-16, -32, 63);
    ChunkPosition cp(bp);
    ChunkPosition cp_r(-2,-3);
    ASSERT_EQ(cp_r, cp);
}

TEST(PositionTests, ChunkPosNegRand)
{
    BlockPosition bp(-7, -90, 99);
    ChunkPosition cp(bp);
    ChunkPosition cp_r(-1,-6);
    ASSERT_EQ(cp_r, cp);
}

}
