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

#include <iosfwd>
#include <stdint.h>

namespace xim {

/// Represents the value of a block
struct Block
{
    uint8_t type;
    uint8_t metadata;
    uint8_t blocklight;
    uint8_t skylight;

    Block(Block const&) = default;
    Block& operator=(Block const&) = default;

    bool operator==(Block const& rhs) const {
        return type == rhs.type &&
               metadata == rhs.metadata &&
               blocklight == rhs.blocklight &&
               skylight == rhs.skylight;
    }
};

class Chunk
{
public:
    Chunk();
    ~Chunk();

    enum {
        size_x = 16,
        size_z = 16,
        size_y = 128
    };

    friend std::ostream& operator<<(std::ostream& os, Chunk const& chunk);

    /// Get the block at the specified coordinates
    Block get(unsigned int x, 
              unsigned int z,
              unsigned int y) const;

    /// Set block at specified coordinates
    void set(unsigned int x,
             unsigned int z,
             unsigned int y,
             Block const& block);

private:
    void allocate();
    void free();

    uint8_t *blocks;
    uint8_t *metadata;
    uint8_t *blocklight;
    uint8_t *skylight;
};

std::ostream& operator<<(std::ostream& os, Chunk const& chunk);

} // namespace xim

