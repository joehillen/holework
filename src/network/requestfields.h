// requestfields.h
//
// Functions for reading request fields from buffers
//
#pragma once

#include <boost/asio.hpp>
#include <string>

#include "inetconv.h"

namespace boostcraft { namespace network {

/**
 * Function: readNumber
 *
 * Reads a primitive numeric type from a buffer, if that buffer contains enough
 * data for a single value of that numeric type. Returns the number of bytes
 * still needed to complete the value.
 */
template<typename T>
size_t readNumber(boost::asio::streambuf& buf, T& out)
{
    using namespace boost::asio;

    const size_t required = sizeof(T);

    if (buf.size() >= required)
    {
        const T* const data = buffer_cast<const T*>(buf.data());
        out = ntoh(*data);
        buf.consume(required);
        return 0;
    }
    return required - buf.size();
}

/**
 * Specialized numeric readers
 */
size_t readByte(boost::asio::streambuf&, int8_t&);
size_t readShort(boost::asio::streambuf&, int16_t&);
size_t readInt(boost::asio::streambuf&, int32_t&);
size_t readLong(boost::asio::streambuf&, int64_t&);
size_t readFloat(boost::asio::streambuf&, float&);
size_t readDouble(boost::asio::streambuf&, double&);

/**
 * Function: readString
 *
 * Reads a string from a buffer and performs a specified conversion.
 */
template<
        typename T,
        std::string Converter(std::basic_string<T> const&)
        /* TODO: add maxlen template parameter */
    >
size_t readString(boost::asio::streambuf& buf, std::string& out)
{
    using namespace boost::asio;

    const size_t available = buf.size();

    // First we need two bytes for length tag
    if (available < 2)
        return 2 - available;

    size_t chars_needed = ntoh(*buffer_cast<const int16_t*>(buf.data()));
    size_t required = 2 + chars_needed * sizeof(T);

    // Then we need the rest of the string
    if (available < required)
        return required - available;

    buf.consume(2);
    std::basic_string<T> temp(
        buffer_cast<const T*>(buf.data()), 
        chars_needed);
    out = Converter(temp);

    // Consume everything
    buf.consume(required - 2);
    return 0;
}

/**
 * Function: readStringUtf8
 *
 * Convenience function; alias of readString<char, NullConverter>
 */
size_t readStringUtf8(boost::asio::streambuf&, std::string&);

/**
 * Function: readStringUcs2
 *
 * Convenience function; alias of readString<char, UCS2Converter>
 */
size_t readStringUcs2(boost::asio::streambuf&, std::string&);

}} // namespace boostcraft::network
