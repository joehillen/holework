// ioservice.h
//
#pragma once
#include <boost/asio.hpp>

namespace boostcraft
{
    /**
     * Function: io_service()
     *
     * Returns a reference to the io_service object for the boostcraft system.
     *
     * Various parts of the boostcraft system can use this object to schedule
     * tasks, create timers, or perform other asynchronous activities.
     *
     * Boostcraft is run by executing io_service::run in (potentially) several
     * threads; all tasks are done asynchronously through the io_service, which
     * means that tasks might be picked up by any thread that is executing
     * io_service::run. This means: minimize shared mutable state if you can, and
     * use boost::asio::strand for any callbacks that access state that must be
     * synchronized (or else do your own locking of entire objects with mutexes,
     * but beware the vile deadlock beast!).
     */
    boost::asio::io_service& io_service();
}

