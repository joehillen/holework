// packetbuf.h

#include "packet.h"
#include "packetfield.h"

#include <list>
#include <boost/system/error_code.hpp>
#include <boost/asio.hpp>

/*
 * PacketParser facilitates the reading of Minecraft packets with boost::asio
 * read() or async_read(). A PacketParser's buffer and completion function can
 * be provided in the asio read call, and then the read handler will be called
 * as soon as the PacketParser has parsed one complete Minecraft packet.
 */
class PacketParser
{
public:
    PacketParser();

    boost::asio::streambuf& buffer();

    size_t done(const boost::system::error_code& error, size_t bytes_read);

private:
    void parse();

    boost::asio::streambuf buffer_;

    Packet::pointer packet;
    std::list<PacketField::pointer> fieldList;
};
