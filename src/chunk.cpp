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

namespace boostcraft {

Chunk::Chunk()
{
    allocate();
}

Chunk::~Chunk()
{
    free();
}

void Chunk::allocate()
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
    // set all y=0 to smooth stone
    for(int x = 0; x < size_x; ++x)
    {
        for(int z = 0; z < size_z; ++z)
        {
            int index = z * size_y + (x * size_y * size_z);
            this->blocks[index] = 12;
            if (x == 0 || x == size_x-1 || z == 0 || z == size_z-1)
                this->blocks[index+1] = 25;
            this->skylight[index/2] = 0xff;
            this->skylight[index/2+1] = 0xff;
        }
    }
}

void Chunk::free()
{
    delete[] blocks;
    delete[] metadata;
    delete[] blocklight;
    delete[] skylight;
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

} // namespace boostcraft

