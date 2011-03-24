// response.cpp

#include "response.h"
#include <ostream>
#include <netinet/in.h>

uint64_t htonll(uint64_t n)
{
    return (((uint64_t)htonl(n)) << 32) | (htonl(n >> 32));
}

Response& operator<<(Response& os, uint8_t n)
{
    std::ostream out(os.data.get());
    out << n;
    return os;
}

Response& operator<<(Response& os, uint16_t n)
{
    std::ostream out(os.data.get());
    out << htons(n);
    return os;
}

Response& operator<<(Response& os, uint32_t n)
{
    std::ostream out(os.data.get());
    out << htonl(n);
    return os;
}

Response& operator<<(Response& os, uint64_t n)
{
    std::ostream out(os.data.get());
    out << htonll(n);
    return os;
}

Response& operator<<(Response& os, std::string const& s)
{
    std::ostream out(os.data.get());
    out << htons(s.length());
    out << s;
    return os;
}


Response chatmessage(std::string const& msg)
{
    Response r;
    r << (uint8_t)RESPONSE_CHAT;
    r << msg;
    return r;
}


