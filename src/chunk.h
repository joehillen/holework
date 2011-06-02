// chunk.h

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

#pragma once

#include <ostream>
#include <stdint.h>
#include <cstring>

namespace boostcraft {

class Chunk
{
public:
    Chunk()
    {
        allocate();
    }

    ~Chunk()
    {
        free();
    }
        
    enum {
        size_x = 16,
        size_z = 16,
        size_y = 128
    };

    friend std::ostream& operator<<(std::ostream& os, Chunk const& chunk);

private:
    void allocate() {
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
                this->blocks[index+1] = 25;
                this->skylight[index/2] = 0xff;
            }
        }
    }

    void free() {
        delete[] blocks;
        delete[] metadata;
        delete[] blocklight;
        delete[] skylight;
    }

    uint8_t *blocks;
    uint8_t *metadata;
    uint8_t *blocklight;
    uint8_t *skylight;
};

std::ostream& operator<<(std::ostream& os, Chunk const& chunk);

} // namespace boostcraft

