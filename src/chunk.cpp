// chunk.cpp

/***********************************************************************
* Copyright (C) 2011 Holework Project
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Affero General Public License as
* published by the Free Software Foundation, either version 3 of the
* License, or (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Affero General Public License for more details.
* 
* You should have received a copy of the GNU Affero General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*************************************************************************/

#include "chunk.h"

#include <ostream>
#include <cstring>
#include <gtest/gtest.h>
#include <stdexcept>

namespace xim {

Chunk::Chunk()
{
    int size = size_x * size_z * size_y;
    this->blocks = new uint8_t[size];
    this->metadata = new uint8_t[size/2];
    this->blocklight = new uint8_t[size/2];
    this->skylight = new uint8_t[size/2];

    std::memset(this->blocks, 0, size);
    std::memset(this->metadata, 0, size/2);
    std::memset(this->blocklight, 0, size/2);
    std::memset(this->skylight, 0, size/2);
}

Chunk::~Chunk()
{
    delete[] blocks;
    delete[] metadata;
    delete[] blocklight;
    delete[] skylight;
}

Block Chunk::get(unsigned int x, unsigned int z, unsigned int y) const
{
    if (x >= size_x || z >= size_z || y >= size_y)
        throw std::out_of_range("block index out of range");

    int index = y + z * size_y + (x * size_y * size_z);

    uint8_t type = this->blocks[index];
    uint8_t metadata;
    uint8_t blocklight;
    uint8_t skylight;

    auto get_upper_nibble = [](uint8_t whole)
    {
        return (whole & 0xF0) >> 4;
    };

    auto get_lower_nibble = [](uint8_t whole)
    {
        return (whole & 0x0F);
    };

    if (index % 2 == 0)
    {
        metadata = get_upper_nibble(this->metadata[index/2]);
        blocklight = get_upper_nibble(this->blocklight[index/2]);
        skylight = get_upper_nibble(this->skylight[index/2]);
    } 
    else 
    {
        metadata = get_lower_nibble(this->metadata[index/2]);
        blocklight = get_lower_nibble(this->blocklight[index/2]);
        skylight = get_lower_nibble(this->skylight[index/2]);
    }
    return Block { type, metadata, blocklight, skylight };
}

void Chunk::set(unsigned int x,
                unsigned int z,
                unsigned int y,
                Block const& block)
{
    if (x >= size_x || z >= size_z || y >= size_y) 
    {
        throw std::out_of_range("block index out of range");
    }

    int index = y + z * size_y + (x * size_y * size_z);
    this->blocks[index] = block.type;

    auto set_upper_nibble = [](uint8_t& whole, uint8_t half)
    {
        whole = (whole & 0x0F) | (half << 4);
    };

    auto set_lower_nibble = [](uint8_t& whole, uint8_t half)
    {
        whole = (whole & 0xF0) | half;
    };

    if (index % 2 == 0)
    {
        set_upper_nibble(metadata[index/2], block.metadata);
        set_upper_nibble(blocklight[index/2], block.blocklight);
        set_upper_nibble(skylight[index/2], block.skylight);
    } 
    else 
    {
        set_lower_nibble(metadata[index/2], block.metadata);
        set_lower_nibble(blocklight[index/2], block.blocklight);
        set_lower_nibble(skylight[index/2], block.skylight);
    }
}

std::ostream& operator<<(std::ostream& os, Chunk const& chunk)
{
    size_t size = Chunk::size_x * Chunk::size_y * Chunk::size_z;
    // we would be in serious trouble if sizeof(char) != sizeof(uint8_t) :D
    os.write((const char*)chunk.blocks, size);
    os.write((const char*)chunk.metadata, size/2);
    os.write((const char*)chunk.blocklight, size/2);
    os.write((const char*)chunk.skylight, size/2);
    return os;
}

} // namespace xim

// UNIT TESTS
std::ostream& operator<<(std::ostream& os, xim::Block const& block)
{
    os << "Block(type=" << (int)block.type
       << ", meta=" << (int)block.metadata
       << ", blocklight=" << (int)block.blocklight
       << ", skylight=" << (int)block.skylight
       << ")";
    return os;
}

TEST(ChunkTests, SetAndGet)
{
    using namespace xim;

    Chunk chunk;
    Block b { 42, 11, 12, 13 };
    chunk.set(0, 0, 0, b);
    chunk.set(1, 1, 1, b);
    chunk.set(15, 15, 127, b);
    ASSERT_EQ(b, chunk.get(1, 1, 1));
    ASSERT_EQ(b, chunk.get(0, 0, 0));
    ASSERT_EQ(b, chunk.get(15, 15, 127));
}

TEST(ChunkTests, GetOutOfRange)
{
    using namespace xim;
    Chunk chunk;
    ASSERT_THROW(chunk.get(0,0,128), std::out_of_range);
    ASSERT_THROW(chunk.get(-12,0,50), std::out_of_range);
    ASSERT_THROW(chunk.get(24,15,10), std::out_of_range);
    ASSERT_THROW(chunk.get(0,12,9001), std::out_of_range);

    ASSERT_NO_THROW(chunk.get(15,15,127));
    ASSERT_NO_THROW(chunk.get(0,0,0));
}

TEST(ChunkTests, SetOutOfRange)
{
    using namespace xim;
    Chunk chunk;
    Block b { 119, 7, 1, 0 };
    ASSERT_THROW(chunk.set(0,0,128,b), std::out_of_range);
    ASSERT_THROW(chunk.set(16,5,50,b), std::out_of_range);
    ASSERT_THROW(chunk.set(9001,9,12,b), std::out_of_range);
    ASSERT_THROW(chunk.set(0,-1,0,b), std::out_of_range);
    ASSERT_THROW(chunk.set(142,252,571,b), std::out_of_range);

    ASSERT_NO_THROW(chunk.set(15,15,127,b));
    ASSERT_NO_THROW(chunk.set(0,0,0,b));
}

