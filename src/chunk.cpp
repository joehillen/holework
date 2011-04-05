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

namespace boostcraft {

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

