// packetfield.h

#include <sys/types.h>
#include <boost/shared_ptr.hpp>

#include <netinet/in.h>

/**
 * A PacketField represents a field in a packet during parsing. Supports
 * reading from a streambuf and continuing the read later if the buffer
 * did not contain a complete object.
 */
class PacketField
{
    typedef boost::shared_ptr<PacketField> pointer;

    /**
     * Attempts to read the field from the given stream; returns the number of
     * bytes that still need to be read.
     */
    virtual int readFrom(boost::asio::streambuf& buf) = 0;


    virtual bool done() = 0;
};

class IntField : public PacketField
{
    static Packet::pointer create(int32_t& out)
    {
        return Packet::pointer(new IntField(out));
    }

    IntField(int32_t& out)
        : out_(out)
    {
    }

    int readFrom(boost::asio::streambuf& buf)
    {
        // Consume at most 4 bytes
        int available = min(4, buf.size());

        if(available >= 4)
        {
            out_ = ntohl(boost::asio::buffer_cast<const uint32_t*>(buf)[0]);
            buf.consume(4);
        }

        return max(0, 4 - available);
    }

private:
    int32_t& out_;
};

/*
 * TODO: move this to a separate header
 */
uint64_t ntohll(uint64_t n)
{
    // TODO: return n if host byte order is big-endian
    return (n>>56) | 
        ((n<<40) & 0x00FF000000000000) |
        ((n<<24) & 0x0000FF0000000000) |
        ((n<<8)  & 0x000000FF00000000) |
        ((n>>8)  & 0x00000000FF000000) |
        ((n>>24) & 0x0000000000FF0000) |
        ((n>>40) & 0x000000000000FF00) |
        (n<<56);
}

class LongField : public PacketField
{
    static Packet::pointer create(int64_t& out)
    {
        return Packet::pointer(new LongField(out));
    }

    LongField(int64_t& out)
        : out_(out)
    {
    }

    int readFrom(boost::asio::streambuf& buf)
    {
        int available = min(8, buf.size());

        if (available >= 8)
        {
            out_ = ntohll(boost::asio::buffer_cast<const uint64_t*>(buf)[0]);
            buf.consume(8);
        }

        return max(0, 8 - available);
    }
};
