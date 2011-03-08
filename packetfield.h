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

// TODO: move to implementation file

#define MIN(a,b) ((a)>(b)?(b):(a))
#define MAX(a,b) ((a)<(b)?(b):(a))


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

class StringField : public PacketField
{
public:
    static PacketField::pointer create(std::string& out)
    {
        return PacketField::pointer(new StringField(out, DEFAULT_MAXLEN));
    }

    static PacketField::pointer create(std::string& out, int maxlen)
    {
        return PacketField::pointer(new StringField(out, maxlen));
    }

    StringField(std::string& out, int maxlen)
        : out_(out), state_(NEED_LENGTH), length_(0)
    {
    }

    int readFrom(boost::asio::streambuf& buf)
    {
        using namespace boost::asio;
        /*
         * StringField can be in three states:
         *  - need length
         *  - need rest of string
         *  - done
         */


        //
        if (state_ == DONE)
        {
            length_ = 0;
            state_ = NEED_LENGTH;
        }

        if (state_ == NEED_LENGTH)
        {
            int available = MIN(2, buf.size());
            if (available >= 2)
            {
                length_ = ntohs(buffer_cast<const uint16_t*>(buf.data())[0]);
                out_.resize(length_);
                buf.consume(2);
                state_ = NEED_DATA;
            }
            else
            {
                return 2 - available;
            }
        }

        if (state_ == NEED_DATA)
        {
            int available = MIN(length_, buf.size());
            if (available >= length_)
            {
                out_.assign(buffer_cast<const char*>(buf.data()), length_);
                buf.consume(length_);
                state_ = DONE;
            }

            return MAX(0, length_ - available);
        }
    }

private:
    std::string& out_;

    enum state {
        NEED_LENGTH,
        NEED_DATA,
        DONE
    } state_;
    int length_;
};


