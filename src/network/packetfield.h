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
#include <netinet/in.h>
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

class ByteField : public PacketField
{
public:
    static PacketField::pointer create(uint8_t& out)
    {
        return PacketField::pointer(new ByteField(out));
    }

    ByteField(uint8_t& out)
        : out_(out)
    {
    }

    int readFrom(boost::asio::streambuf& buf)
    {
        int available = MIN(1, buf.size());
        if (available >= 1)
        {
            out_ = boost::asio::buffer_cast<const uint8_t*>(buf.data())[0];
            buf.consume(1);
        }
        return MAX(0, 1 - available);
    }

private:
    uint8_t& out_;
};

class ShortField : public PacketField
{
public:
    static PacketField::pointer create(uint16_t& out)
    {
        return PacketField::pointer(new ShortField(out));
    }

    ShortField(uint16_t& out)
        : out_(out)
    {
    }

    int readFrom(boost::asio::streambuf& buf)
    {
        int available = MIN(2, buf.size());
        if (available >= 2)
        {
            out_ = ntohs(boost::asio::buffer_cast<const uint16_t*>(buf.data())[0]);
        }
        return MAX(0, 1 - available);
    }

private:
    uint16_t& out_;
};

class IntField : public PacketField
{
public:
    static PacketField::pointer create(uint32_t& out)
    {
        return PacketField::pointer(new IntField(out));
    }

    IntField(uint32_t& out)
        : out_(out)
    {
    }

    int readFrom(boost::asio::streambuf& buf)
    {
        // Consume at most 4 bytes
        int available = MIN(4, buf.size());

        if(available >= 4)
        {
            out_ = ntohl(boost::asio::buffer_cast<const uint32_t*>(buf.data())[0]);
            buf.consume(4);
        }

        return MAX(0, 4 - available);
    }

private:
    uint32_t& out_;
};

/*
 * TODO: move this to a separate header
 */
static uint64_t ntohll(uint64_t n)
{
    // TODO: return n if host byte order is big-endian
    return (n>>56) | 
        ((n<<40) & 0x00FF000000000000LLU) |
        ((n<<24) & 0x0000FF0000000000LLU) |
        ((n<<8)  & 0x000000FF00000000LLU) |
        ((n>>8)  & 0x00000000FF000000LLU) |
        ((n>>24) & 0x0000000000FF0000LLU) |
        ((n>>40) & 0x000000000000FF00LLU) |
        (n<<56);
}

class LongField : public PacketField
{
public:
    static PacketField::pointer create(uint64_t& out)
    {
        return PacketField::pointer(new LongField(out));
    }

    LongField(uint64_t& out)
        : out_(out)
    {
    }

    int readFrom(boost::asio::streambuf& buf)
    {
        int available = MIN(8, buf.size());

        if (available >= 8)
        {
            out_ = ntohll(boost::asio::buffer_cast<const uint64_t*>(buf.data())[0]);
            buf.consume(8);
        }

        return MAX(0, 8 - available);
    }

private:
    uint64_t& out_;
};

class BoolField : public PacketField
{
public:
    static PacketField::pointer create(uint8_t& out)
    {
        return PacketField::pointer(new BoolField(out));
    }

    BoolField(uint8_t& out)
        : out_(out)
    {
    }

    int readFrom(boost::asio::streambuf& buf)
    {
        int available = MIN(1, buf.size());
        if (available >= 1)
        {
            out_ = boost::asio::buffer_cast<const uint8_t*>(buf.data())[0];
            buf.consume(1);
        }
        return MAX(0, 1 - available);
    }

private:
    uint8_t& out_;
};

class FloatField : public PacketField
{
public:
    static PacketField::pointer create(float& out)
    {
        return PacketField::pointer(new FloatField(out));
    }

    FloatField(float& out)
        : out_(out)
    {
    }

    int readFrom(boost::asio::streambuf& buf)
    {
        int available = MIN(4, buf.size());
        if (available >= 4)
        {
            out_ = boost::asio::buffer_cast<const float*>(buf.data())[0];
            buf.consume(4);
        }
        return MAX(0, 4 - available);
    }

private:
    float& out_;
};

class DoubleField : public PacketField
{
public:
    static PacketField::pointer create(double& out)
    {
        return PacketField::pointer(new DoubleField(out));
    }

    DoubleField(double& out)
        : out_(out)
    {
    }

    int readFrom(boost::asio::streambuf& buf)
    {
        int available = MIN(8, buf.size());
        if (available >= 1)
        {
            out_ = boost::asio::buffer_cast<const double*>(buf.data())[0];
            buf.consume(8);
        }
        return MAX(0, 8 - available);
    }

private:
    double& out_;
};


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
                length_needed_ = 2*ntohs(buffer_cast<const uint16_t*>(buf.data())[0]);
                temp_.resize(length_needed_);
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

            temp_.append(buffer_cast<const char16_t*>(buf.data()), available);
            buf.consume(available);

            length_needed_ -= available;
        }

        if (length_needed_ == 0)
        {
            out_ = ucs2toutf8(temp_);
        }
        return length_needed_;
    }
    
private:
    std::string& out_;
    std::u16string temp_;

    int length_needed_;
};


}} //end namespace boostcraft::network

