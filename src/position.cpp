
#include "position.h"
#include "chunk.h"
#include <math.h>
#include <ostream>
#include <gtest/gtest.h>

namespace xim {

EntityPosition operator+(EntityPosition const& a, EntityPosition const& b)
{
    return EntityPosition(a.x + b.x, a.z + b.z, a.y + b.y);
}

std::ostream& operator<<(std::ostream& os, xim::EntityPosition const& pos)
{
    os << "EntityPosition(x=" << pos.x
        << ", z=" << pos.z
        << ", y=" << pos.y
        << ")";
    return os;
}

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
    auto algo = [&](int const& p, int const& size)
    {
        int r = p / size;
        if (p < 0)
        {
            if (p % size != 0)
            {
                r -= 1;
            }
        }
        return r;
    };

    x = algo(pos.x, Chunk::size_x);
    z = algo(pos.z, Chunk::size_z);
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

TEST(PositionTests, EntityPosAddZero)
{
    EntityPosition a(0, 0, 0);
    EntityPosition b(0, 0, 0);
    EntityPosition sum = a + b;
    EntityPosition result(0,0,0);
    ASSERT_EQ(result, sum);
}

TEST(PositionTests, EntityPosAddOne)
{
    EntityPosition a(0, 0, 0);
    EntityPosition b(1, 1, 1);
    EntityPosition sum = a + b;
    EntityPosition result(1,1,1);
    ASSERT_EQ(result, sum);
}

TEST(PositionTests, EntityPosAddRand)
{
    EntityPosition a(19.12, 12813.0183, 100);
    EntityPosition b(7, 9.12, 0.00123);
    EntityPosition sum = a + b;
    EntityPosition result(26.12, 12822.1383, 100.00123);
    ASSERT_EQ(result, sum);
}

TEST(PositionTests, EntityPosAddNegOne)
{
    EntityPosition a(1, 1, 1);
    EntityPosition b(-1, -1, -1);
    EntityPosition sum = a + b;
    EntityPosition result(0,0,0);
    ASSERT_EQ(result, sum);
}

TEST(PositionTests, EntityPosAddNegRand)
{
    EntityPosition a(13, 19, 45);
    EntityPosition b(-100.1, -1, -5.5);
    EntityPosition sum = a + b;
    EntityPosition result(-87.1,18,39.5);
    ASSERT_EQ(result, sum);
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
    BlockPosition bp(0, -1, 103);
    ChunkPosition cp(bp);
    ChunkPosition cp_r(0,-1);
    ASSERT_EQ(cp_r, cp);
}

TEST(PositionTests, ChunkPosNegOne)
{
    BlockPosition bp(-16, -15, 63);
    ChunkPosition cp(bp);
    ChunkPosition cp_r(-1,-1);
    ASSERT_EQ(cp_r, cp);
}

TEST(PositionTests, ChunkPosNegTwo)
{
    BlockPosition bp(-17, -32, 63);
    ChunkPosition cp(bp);
    ChunkPosition cp_r(-2,-2);
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
