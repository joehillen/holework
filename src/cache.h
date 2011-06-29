// cache.h
//
#pragma once


namespace boostcraft
{
    // Forward Declarations
    class Chunk;
    class ChunkPosition;
    namespace event {
        class NewChunkEvent;
    }

    const unsigned int MAX_CACHE = 10000;

    class ChunkCache
    {
    public:
        std::shared_ptr<Chunk> get(ChunkPosition const& p);
        void add(ChunkPosition const& p, std::shared_ptr<Chunk> chunk);
        void remove(ChunkPosition const& p);
        void handler(event::NewChunkEvent& e);
    private:
        // HASH FUNCTION, Y U SO UGLY!?
        template<typename T> struct hash
        {
            size_t operator()(ChunkPosition const& p) const
            {
                // Cantor pairing function
                return 0.5*(p.x + p.z)*(p.x + p.z + 1) + p.z;
            }
        };
        void move_front(ChunkPosition const& p);
        std::list<ChunkPosition> list;
        std::unordered_map< ChunkPosition,
                            std::shared_ptr<Chunk>,
                            hash<ChunkPosition>> map;
    };
}

