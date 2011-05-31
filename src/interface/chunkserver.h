// chunkserver.h
//  Abstract interface to a seamless interface to a world of chunks
//
#pragma once

// Forward declarations
namespace boostcraft {
    namespace map {
        class Chunk;    // TODO: Chunk to be moved to map namespace
        class Block;
    }
}

namespace boostcraft { namespace interface
{
    /**
     * A ChunkServer is where the World goes to get its chunk fix. It provides
     * a way for the World to retrieve chunks or specific blocks, and allows
     * the World to notify it of changes to be committed.
     *
     * The design intent is that a ChunkServer use a ChunkGenerator and a
     * ChunkStore to provide for the creation and storage of chunks; then the
     * responsibility of ChunkServer is mainly to maintain a cache of chunks
     * for efficient access and modification.
     *
     * In ordinary circumstances a map data request should never fail; if the
     * target chunk does not exist (e.g., in a ChunkStore) it should be created
     * (e.g., with a ChunkGenerator).
     *
     * TODO: maybe this should be renamed MapServer?
     *   It seems possible that this will be the point at which "Chunks" are
     *   hidden by abstraction. The only time we really need to deal with
     *   entire chunks is when sending them to the player...
     */
    class ChunkServer
    {
    public:
        /**
         * Function: blockAt
         *
         * Returns the block at the specified coordinates.
         */
        virtual map::Block blockAt(int32_t x, int32_t y, int32_t z) = 0;

        /**
         * Function: setBlockAt
         *
         * Changes the block at the specified coordinates.
         */
        virtual void setBlockAt(int32_t x, int32_t y, int32_t z, Block b) = 0;

        /**
         * Function: commitAll
         *
         * Flushes any pending changes to persistent storage, if applicable.
         * TODO: is this a reasonable responsibility for the Chunk/MapServer?
         * If Chunk caching occurs at this level in the architecture, then yes;
         * but cache flushing seems mainly connected to persistent storage,
         * which is nominally ChunkStore's job.
         *
         * Still, the server needs a way to ensure that cached changes are
         * safely stored.
         *
         * Actually, RAII should be able to handle that. If the MapServer holds
         * a cache, it should flush it in its destructor. We probably don't
         * need this function.
         */
        virtual void commitAll() = 0;
    };

}} // namespace boostcraft::interface
