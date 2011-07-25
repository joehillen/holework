// world.h
//  Abstract world interface
//
#pragma once

// Forward declarations
namespace xim {
}

namespace xim { namespace interface
{
    class World
    {
    public:
        virtual int8_t blockTypeAt(int x, int y, int z) = 0; 
    };

}} // namespace xim::interface

