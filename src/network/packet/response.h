// response.h
#pragma once

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>


enum ResponseType
{
    RESPONSE_KEEP_ALIVE         = 0,
    RESPONSE_LOGIN              = 1,
    RESPONSE_HANDSHAKE          = 2,
    RESPONSE_CHAT               = 3,
    RESPONSE_UPDATE_TIME        = 4,
    RESPONSE_ENTITY_EQUIPMENT   = 5,
    RESPONSE_SPAWN_POS          = 6,
    RESPONSE_UPDATE_HEALTH      = 7,
    RESPONSE_POSITION_LOOK      = 0xD,
    RESPONSE_NAMED_ENTITY_SPAWN = 0x14,
    RESPONSE_PICKUP_SPAWN       = 0x15,
    RESPONSE_COLLECT_ITEM       = 0x16,
    RESPONSE_ADD_OBJECT         = 0x17,
    RESPONSE_MOB_SPAWN          = 0x18,
    RESPONSE_ENTITY_PAINTING    = 0x19,
    // TODO: add the rest of this stuff
};

struct Response
{
    boost::shared_ptr<boost::asio::streambuf> data;
    
    Response() 
        : data(new boost::asio::streambuf(64))
    {
    }

    boost::asio::streambuf& buffer()
    {
        return *this->data;
    }
};

Response& operator<<(Response& os, uint8_t n);
Response& operator<<(Response& os, uint16_t n);
Response& operator<<(Response& os, uint32_t n);
Response& operator<<(Response& os, uint64_t n);
Response& operator<<(Response& os, std::string const& s);

/*
 * Response construction functions
 */
Response chatmessage(std::string const& msg);
