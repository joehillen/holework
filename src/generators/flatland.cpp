// generators.cpp

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

#include "generators.h"
#include "chunk.h"
#include "typedefs.h"
#include "event/event.h"
#include "event/types.h"
#include "log.h"

namespace xim {

namespace generators {

void flatland(event::NeedChunkEvent& e)
{
    log(DEBUG,"flatland","Generating Chunk");
    using namespace event;

    chunk_ptr chunk(new Chunk);
    
    auto plane = [&](unsigned int y, uint8_t blocktype)
    {
        for (unsigned int x = 0; x < chunk->size_x; ++x)
        {
            for (unsigned int z = 0; z < chunk->size_z; ++z)
            {
                Block b = { 
                    blocktype,
                    (uint8_t)0,  //metadata
                    (uint8_t)15, //blocklight
                    (uint8_t)15  //skylight
                };
                chunk->set(x, z, y, b);
            }
        }
    };
    
    for (unsigned int y = 0; y < chunk->size_y; ++y)
    {
        if (y < 2) 
        {
            plane(y, 7);
        }
        else if (y < 34) 
        {
            plane(y, 1);
        }
        else if (y < 63) {
            plane(y, 3);
        }
        else if (y < 64) 
        {
            plane(y, 2);
        }
        else
        {
            plane(y, 0);
        }
    }

    async_fire(NewChunkEvent(e.world, e.x, e.z, chunk));
}

} // namespace generators
} // namespace xim

