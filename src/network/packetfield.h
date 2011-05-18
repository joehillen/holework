// packetfield.h

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

#pragma once

#include <sys/types.h>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <string>

#include "inetconv.h"

#define MIN(a,b) ((a)>(b)?(b):(a))
#define MAX(a,b) ((a)<(b)?(b):(a))

namespace boostcraft { namespace network { 

/**
 * A PacketField represents a field in a packet during parsing. Supports
 * reading from a streambuf and continuing the read later if the buffer
 * did not contain a complete object.
 */
class PacketField
{
public:
    typedef boost::shared_ptr<PacketField> pointer;

    /**
     * Attempts to read the field from the given stream; returns the number of
     * bytes that still need to be read.
     */
    virtual int readFrom(boost::asio::streambuf& buf) = 0;
};


/**
 * Template for numeric packet fields
 */
template<typename T>
class NumField : public PacketField
{
public:
    static PacketField::pointer create(T& out)
    {
        return PacketField::pointer(new NumField<T>(out));
    }

    NumField(T& out) : out_(out)
    {
    }

    enum {
        required = sizeof(T)
    };

    int readFrom(boost::asio::streambuf& buf)
    {
        int available = MIN(required, buf.size());
        if (available >= required)
        {
            out_ = ntoh(boost::asio::buffer_cast<const T*>(buf.data())[0]);
            buf.consume(required);
        }
        return MAX(0, required - available);
    }

private:
    T& out_;
};

typedef NumField<uint8_t> ByteField;
typedef NumField<uint8_t> BoolField;
typedef NumField<uint16_t> ShortField;
typedef NumField<uint32_t> IntField;
typedef NumField<uint64_t> LongField;
typedef NumField<float> FloatField;
typedef NumField<double> DoubleField;


/**
 * Template class: StringField
 *
 * Specializations of this class represent string fields which convert their
 * underlying network data from some format to UTF-8 std::strings. Boostcraft
 * rolls with UTF-8 all the way because it's the only thing that makes sense.
 * Java and its UCS-2 can suck it.
 *
 * This template has two parameters:
 *
 *   T          Character type of the underlying string data.
 *   Converter  Function that converts from the underlying encoding to UTF-8.
 *
 * These two parameters should allow the definition of a string field class
 * for whatever silly string encodings Notch decides in his boundless wisdom
 * to employ.
 */
template<typename T, std::string Converter(std::basic_string<T> const&)>
class StringField : public PacketField
{
public:
    static PacketField::pointer create(std::string& out)
    {
        return PacketField::pointer(new StringField<T,Converter>(out));
    }
    
    static PacketField::pointer create(std::string& out, size_t maxlen)
    {
        // TODO: implement maxlen
        return create(out);
    }

    enum {
        NO_LENGTH_SET = 0xffffffff
    };

    StringField(std::string& out)
        : out_(out), chars_needed_(NO_LENGTH_SET)
    {
    }

    StringField(std::string& out, size_t maxlen)
        : out_(out), chars_needed_(NO_LENGTH_SET)
    {
    }

    /**
     * Attempts to read the string from the given buffer; will consume whatever
     * is available, allowing large strings to be read in successive reads of
     * small buffers.
     */
    int readFrom(boost::asio::streambuf& buf)
    {
        using namespace boost::asio;

        // First read the length tag
        if (chars_needed_ == NO_LENGTH_SET)
        {
            unsigned available = MIN(2, buf.size());
            if (available >= 2)
            {
                chars_needed_ = ntoh(
                    buffer_cast<const uint16_t*>(buf.data())[0]);
                temp_.reserve(chars_needed_);
                buf.consume(2);
            }
            else
            {
                return 2 - available;
            }
        }

        // Then read as much data as needed/possible
        if (chars_needed_ > 0)
        {
            // Make sure that available registers only complete chars
            unsigned available = MIN(chars_needed_, buf.size() / sizeof(T));
            temp_.append(buffer_cast<const T*>(buf.data()), available);
            buf.consume(available * sizeof(T));
            chars_needed_ -= available;
        }

        // When everything is done, convert the string
        if (chars_needed_ == 0)
        {
            out_ = Converter(temp_);
            // TODO: PacketFields need a "done" flag; attempting to re-read
            // into a completed field should be a runtime error.
        }

        return chars_needed_ * sizeof(T);
    }

private:
    std::string& out_;
    std::basic_string<T> temp_;
    size_t chars_needed_;
};


std::string NullConverter(std::string const& str);
std::string UCS2Converter(std::u16string const& str);

typedef StringField<char, NullConverter> String8Field;
typedef StringField<char16_t, UCS2Converter> String16Field;


}} //end namespace boostcraft::network

