// cache.cpp

#include <memory>
#include <unordered_map>
#include "event/types.h"
#include "chunk.h"
#include "cache.h"
#include <iostream>

namespace boostcraft
{
    void ChunkCache::remove(ChunkPosition const& p)
    {
        if (map.count(p) > 0)
        {
            list.erase(map[p]);
            map.erase(p);
        }
    }

    void ChunkCache::add(ChunkPosition const& p,
                         std::shared_ptr<Chunk> const chunk)
    {
        ChunkPair pair = std::make_pair(p, chunk);
        list.push_front(pair);

        map[p] = list.begin();

        // Remove old chunks
        while (list.size() > max_size)
        {
            ChunkPosition removed = list.back().first;
            remove(removed);
        }
    }

    std::shared_ptr<Chunk> ChunkCache::get(ChunkPosition const& p)
    {
        if (map.count(p) > 0)
        {
            ChunkList::iterator iter = map[p];
            ChunkPair pair = *iter;
            std::shared_ptr<Chunk> chunk = pair.second;

            // Moved used chunk to the front of the cache
            remove(p);
            add(p, chunk);

            return chunk;
        }
        return std::shared_ptr<Chunk>();
    }


    void ChunkCache::handler(event::NewChunkEvent& e)
    {
        add({e.x, e.z}, e.chunk);
    }
}

