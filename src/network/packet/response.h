// response.h

struct Response
{
    boost::shared_ptr<std::vector<uint8_t> > data;
};

Response chatmessage(std::string const& msg)
{
    Response r;
    r << (uint8_t)RESPONSE_CHAT;
    r << (uint16_t)msg.length();
    r << msg;
    return r;
}
