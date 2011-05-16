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
#include "uniconv.h"

// TODO: move to implementation file

#define MIN(a,b) ((a)>(b)?(b):(a))
#define MAX(a,b) ((a)<(b)?(b):(a))


// Magic Numbers

namespace boostcraft { namespace network { 

const int DEFAULT_MAXLEN  = 100;

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


class String8Field : public PacketField
{
public:
    static PacketField::pointer create(std::string& out, int maxlen)
    {
        return PacketField::pointer(new String8Field(out, maxlen));
    }
    
    static PacketField::pointer create(std::string& out)
    {
        return PacketField::pointer(new String8Field(out, DEFAULT_MAXLEN));
    }
    
    enum {
        NO_LENGTH_SET = -1
    };

    String8Field(std::string& out, int maxlen)
        : out_(out), length_needed_(NO_LENGTH_SET)
    {
    }

    // TODO: this needs to be fixed to read the string in chunks;
    // otherwise string read size is limited by buffer size. Oops! :)
    int readFrom(boost::asio::streambuf& buf)
    {
        using namespace boost::asio;

        if (length_needed_ == NO_LENGTH_SET)
        {
            int available = MIN(2, buf.size());
            if (available >= 2)
            {
                length_needed_ = ntohs(buffer_cast<const uint16_t*>(buf.data())[0]);
                out_.resize(length_needed_);
                buf.consume(2);
            }
            else
            {
                return 2 - available;
            }
        }

        if (length_needed_ > 0)
        {
            unsigned int available = MIN(length_needed_, buf.size());

            out_.append(buffer_cast<const char*>(buf.data()), available);
            buf.consume(available);

            length_needed_ -= available;
        }

        return length_needed_;
    }
    
private:
    std::string& out_;

    int length_needed_;
};


class String16Field : public PacketField
{
public:
    static PacketField::pointer create(std::string& out, int maxlen)
    {
        return PacketField::pointer(new String16Field(out, maxlen));
    }
    
    static PacketField::pointer create(std::string& out)
    {
        return PacketField::pointer(new String16Field(out, DEFAULT_MAXLEN));
    }

    enum {
        NO_LENGTH_SET = -1
    };

    String16Field(std::string& out, int maxlen)
        : out_(out), bytes_needed_(NO_LENGTH_SET)
    {
    }

    // TODO: this needs to be fixed to read the string in chunks;
    // otherwise string read size is limited by buffer size. Oops! :)
    int readFrom(boost::asio::streambuf& buf)
    {
        using namespace boost::asio;

        if (bytes_needed_ == NO_LENGTH_SET)
        {
            int available = MIN(2, buf.size());
            if (available >= 2)
            {
                bytes_needed_ = 2*ntohs(buffer_cast<const uint16_t*>(buf.data())[0]);
                temp_.reserve(bytes_needed_/2);
                buf.consume(2);
            }
            else
            {
                return 2 - available;
            }
        }

        if (bytes_needed_ > 0)
        {
            unsigned int available = MIN(bytes_needed_, buf.size());

            temp_.append(buffer_cast<const char16_t*>(buf.data()), available);
            buf.consume(available);

            bytes_needed_ -= available;
        }

        if (bytes_needed_ == 0)
        {
            out_ = ucs2toutf8(temp_);
        }
        return bytes_needed_;
    }
    
private:
    std::string& out_;
    std::u16string temp_;

    int bytes_needed_;
};


}} //end namespace boostcraft::network

