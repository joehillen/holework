// serialize.cpp

#include "serialize.h"
#include "packet/packet.h" // TODO: response.h refactor?

#include <netinet/in.h>
#include <ostream>

uint64_t htonll(uint64_t n)
{
    // htonl(lower word) << 32 | htonl(upper word)
    return (htonl(n) << 32) | (htonl(n >> 32))
}

std::ostream& operator<<(std::ostream& os, KeepAliveResponse const& packet)
{
    return os;
}

std::ostream& operator<<(std::ostream& os, LoginResponse const& packet)
{
    os << htonl(packet.protocolVersion);
    os << htons(packet.servername.length());
    os << packet.servername;
    os << htons(packet.motd.length());
    os << packet.motd;
    os << htonll(packet.mapSeed);
    os << packet.dimension;
    return os;
}

std::ostream& operator<<(std::ostream& os, HandshakeResponse const& packet)
{
    os << htons(packet.connectionHash.length());
    os << packet.connectionHash;
    return os;
}

std::ostream& operator<<(std::ostream& os, ChatResponse const& packet)
{
    os << htons(packet.message.length());
    os << packet.message;
}

boost::asio::streambuf serialize(Response const& packet)
{
    boost::asio::streambuf buf;

    std::ostream out(&buf);
    out << packet.type;
    out << packet;

    return buf;
}
