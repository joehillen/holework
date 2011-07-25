// cache.cpp

#include <memory>
#include <unordered_map>
#include <gtest/gtest.h>

#include "typedefs.h"
#include "event/types.h"
#include "chunk.h"
#include "position.h"
#include "cache.h"

namespace xim {

void ChunkCache::remove(ChunkPosition const& p)
{
    if (map.count(p) > 0)
    {
        list.erase(map[p]);
        map.erase(p);
    }
}

void ChunkCache::add(ChunkPosition const& p,
                     chunk_ptr const chunk)
{
    ChunkPair pair = std::make_pair(p, chunk);
    if (map.count(p) > 0)
    {
        list.erase(map[p]);            
    }
    list.push_front(pair);
    map[p] = list.begin();

    // Remove old chunks
    while (list.size() > max_size)
    {
        ChunkPosition removed = list.back().first;
        remove(removed);
    }
}

chunk_ptr ChunkCache::get(ChunkPosition const& p)
{
    if (map.count(p) > 0)
    {
        ChunkList::iterator iter = map[p];
        ChunkPair pair = *iter;
        chunk_ptr chunk = pair.second;

        // Moved used chunk to the front of the cache
        remove(p);
        add(p, chunk);

        return chunk;
    }
    return chunk_ptr();
}

void ChunkCache::handler(event::NewChunkEvent& e)
{
    add({e.x, e.z}, e.chunk);
}

/************
*   TESTS   *
************/
TEST(CacheTests, Add)
{
    ChunkCache cache(3);
    chunk_ptr chunk;
    cache.add({0,0}, chunk); 
    cache.add({0,0}, chunk); 
    cache.add({0,0}, chunk); 
}

TEST(CacheTests, Remove)
{
    ChunkCache cache(3);
    chunk_ptr chunk;
    cache.remove({0,0});
    cache.add({0,0}, chunk); 
    cache.add({0,0}, chunk); 
    cache.add({0,0}, chunk); 
    cache.add({1,0}, chunk); 
    cache.add({10000,3}, chunk); 
    cache.add({9,0}, chunk); 
    cache.remove({0,0});
    cache.remove({0,0});
    cache.remove({0,0});
    cache.remove({0,0});
}

TEST(CacheTests, Get)
{
    ChunkCache cache(3);
    chunk_ptr chunk;
    cache.add({0,0}, chunk); 
    ASSERT_EQ(chunk, cache.get({0,0}));
}

} // end namespace xim

