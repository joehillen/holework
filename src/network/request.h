// request.h
//
//
#pragma once

#include <memory>
#include <boost/asio.hpp>

namespace boostcraft { namespace network {

class Request
{
public:
    typedef std::shared_ptr<Request> ptr;
    virtual ~Request() { }

    /**
     * Factory method; returns a new empty Request object of the specified type
     */
    static ptr create(int type);

    /**
     * Virtual method: read
     *
     * Called to read the packet's contents from a streambuf. The buffer may or
     * may not contain enough data to complete the packet. This function should
     * return the number of bytes required to complete the packet, or 0 if the
     * packet buffer contained enough information to complete the packet.
     *
     * Incremental reading is allowed: overrides can consume data from the
     * buffer in any call to read(); the subclass that implements the override
     * is responsible for temporary storage of data taken out of the buffer.
     *
     * Any data that is left in the buffer when read() returns 0 will be
     * interpreted as the start of the next request packet, so the override
     * MUST consume the entire packet before returning 0!
     */
    virtual size_t read(boost::asio::streambuf&) = 0;

    /**
     * Virtual method: dispatch
     *
     * Called when request reading is complete; this method's job is to raise
     * appropriate events in the system when the request packet is received.
     *
     * TODO: if we eliminate global event handling (which may be a good idea),
     *       this will need to take a reference to an event target of some kind
     */
    virtual void dispatch() = 0;

protected:
    int fields_read_;
};

}} // namespace boostcraft::network
