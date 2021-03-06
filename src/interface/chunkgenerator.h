// chunkgenerator.h
//  Interface for chunk generators
//
#pragma once

#include <memory>

// Forward declarations
namespace xim {
    class Chunk;
}

namespace xim {
    namespace interface
{
    class ChunkGenerator
    {
    public:
        /**
         * Function: generate
         *
         * Creates whatever chunk should be at (x,z) chunk coordinates and
         * returns an owning pointer to it.
         */
        virtual std::unique_ptr<Chunk> generate(int32_t x, int32_t z) = 0;
    };
}} // namespace xim::interface

