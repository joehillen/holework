// inetconv.h
#pragma once

//
// Functions for conversion from network to host byte order
//
#include <sys/types.h>
#include <netinet/in.h>

/**
 * Function: isbigendian
 *
 * Returns true iff this machine uses a big endian byte order. Note that this
 * assumes either big endian or little endian. Not guaranteed to work on any
 * machines that practice freaky byte sex.
 */
bool isbigendian();

/**
 * Function: ntohll
 *
 * Converts an unsigned 64-bit integer from network to host byte order.
 */
uint64_t ntohll(uint64_t);


/**
 * Template function: byteswap
 *
 * Swaps byte order of any POD type. Delicious type punning ahead! Note that
 * reading a union member other than the last one written is *technically*
 * undefined; but show me a compiler that doesn't do exactly what you expect
 * here and I'll eat my hat.
 *
 * DISCLAIMER: I do not have a hat.
 */
template<typename T>
T byteswap(T n)
{
    union
    {
        T value;
        char bytes[sizeof(T)];
    } in = {n}, out;

    for(unsigned i = 0; i < sizeof(T); ++i)
    {
        out.bytes[i] = in.bytes[sizeof(T) - i - 1];
    }

    return out.value;
}

/**
 * Template function: ntoh
 *
 * Converts a primitive value from network byte order to host byte order.
 */
template<typename T>
T ntoh(T n)
{
    if (isbigendian())
        return n;
    return byteswap(n);
}



