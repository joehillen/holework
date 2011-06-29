// cache.cpp

#include <memory>
#include <unordered_map>
#include "event/types.h"
#include "chunk.h"
#include "cache.h"

namespace boostcraft
{
    void ChunkCache::move_front(ChunkPosition const& p)
    {
        //TODO: Maybe optimise this a bit. list.remove() has linear complexity. 
        list.remove(p);
        list.push_front(p);
    }
        
    std::shared_ptr<Chunk> ChunkCache::get(ChunkPosition const& p)
    {
        move_front(p);
        return map[p];
    }

    void ChunkCache::remove(ChunkPosition const& p)
    {
        list.remove(p);
        map.erase(p);
    }

    void ChunkCache::add(ChunkPosition const& p,
                         std::shared_ptr<Chunk> const chunk)
    {
        move_front(p);
        map[p] = chunk;

        // Remove old chunks
        while (list.size() > max_size)
        {
            ChunkPosition removed = list.back();
            remove(removed);
        }
    }

    void ChunkCache::handler(event::NewChunkEvent& e)
    {
        add({e.x, e.z}, e.chunk);
    }
}

