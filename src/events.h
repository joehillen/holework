// events.h

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

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/signals2/signal.hpp>

#include "log.h"

namespace boostcraft
{

// Forward declarations
class Player;


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



/**
 * Events in boostcraft are exposed through a set of public classes, one for
 * each type of event that can occur in the system. Each event class includes
 * a static boost::signals2::signal which is used for all instances of that
 * event.
 *
 * To raise an event, create an instance of the desired event class and call
 * fire() on it, e.g.:
 *         LogEvent log_event(INFO, "This is a log message.");
 *         fire(log_event);
 * Alternatively, call async_fire() if you want handling of the event to occur
 * asynchronously.
 *
 * To listen for events, create a function that matches the signature
 * void(EventType&), and then call listen() on it, e.g.:
 *         void logHandler(LogEvent& e) { ... }
 *         listen(logHandler);
 * Your handler will be called whenever the event is fired, unless a handler
 * that ran prior to yours canceled the event. To request that your handler be
 * called even when the event was canceled, use listen_always.
 * 
 */

/**
 * The base class for all events
 */
struct Event
{
    std::string source_name;
    bool canceled;

    Event() : canceled(false) { }
};


struct PlayerEvent : public Event
{
    std::shared_ptr<Player> player;

    PlayerEvent(std::shared_ptr<Player> player) : player(player) { }
};

struct LogEvent : public Event
{
    static boost::signals2::signal<void(LogEvent&)> signal;
    std::string message;
    LogType type;

    LogEvent(boostcraft::LogType type, std::string const& msg) : message(msg), type(type)
    {}
};

struct LoginRequestEvent : public PlayerEvent
{
    static boost::signals2::signal<void(LoginRequestEvent&)> signal;

    int version;
    std::string username;

    LoginRequestEvent(std::shared_ptr<Player> player,
            std::string const& username, int version)
        : PlayerEvent(player), version(version), username(username)
    {
    }
};

struct ChatEvent : public PlayerEvent
{
    static boost::signals2::signal<void(ChatEvent&)> signal;

    std::string message;

    ChatEvent(std::shared_ptr<Player> player, std::string const& msg)
        : PlayerEvent(player), message(msg)
    {
    }
};

struct PlayerNeedsChunkEvent : public PlayerEvent
{
    static boost::signals2::signal<void(PlayerNeedsChunkEvent&)> signal;

    int x;
    int z;

    PlayerNeedsChunkEvent(std::shared_ptr<Player> player, int x, int z)
        : PlayerEvent(player), x(x), z(z)
    {
    }
};

struct PlayerLookEvent : public PlayerEvent
{
    static boost::signals2::signal<void(PlayerLookEvent&)> signal;

    float yaw;
    float pitch;

    PlayerLookEvent(std::shared_ptr<Player> player, float yaw, float pitch)
        : PlayerEvent(player), yaw(yaw), pitch(pitch)
    {
    }
};

struct PlayerPositionEvent : public PlayerEvent
{
    static boost::signals2::signal<void(PlayerPositionEvent&)> signal;

    double x;
    double z;
    double y;

    PlayerPositionEvent(std::shared_ptr<Player> player, double x, double z, double y)
        : PlayerEvent(player), x(x), z(z), y(y)
    {
    }
};

struct PlayerOnGroundEvent : public PlayerEvent
{
    static boost::signals2::signal<void(PlayerOnGroundEvent&)> signal;

    bool on_ground;

    PlayerOnGroundEvent(std::shared_ptr<Player> player, bool on_ground)
        : PlayerEvent(player), on_ground(on_ground)
    {
    }
};

struct PlayerDisconnectEvent : public PlayerEvent
{
    static boost::signals2::signal<void(PlayerDisconnectEvent&)> signal;

    std::string reason;

    PlayerDisconnectEvent(std::shared_ptr<Player> player,
            std::string const& reason)
        : PlayerEvent(player), reason(reason)
    {
    }
};

// signal connection stuff


/**
 * Internal helper: callIfNotCanceled
 *
 * Calls a callback if the given event is uncanceled. Used by wrapper.
 */
template<class EventType>
void callIfNotCanceled(
        boost::function<void(EventType&)> callback,
        EventType& event)
{
    if(!event.canceled)
        callback(event);
}

/**
 * Internal helper: wrapper
 *
 * Wraps an event signal callback so that it will not be called if the event
 * that dispatched it was canceled by a prior signal callback.
 */
template<class EventType>
boost::function<void(EventType&)> wrapper(
        boost::function<void(EventType&)> callback)
{
    return boost::bind(callIfNotCanceled<EventType>, callback, _1);
}


/******************************************************************************
 * Function: listen( callback )
 *
 * Registers an event listener function, to be called whenever the appropriate
 * event is raised (and not canceled). Which event to listen for is inferred
 * from the callback's signature.
 * 
 * The specified callback will NOT be called in response to an event if that
 * event is canceled by a prior handler. To register a listener that will be
 * called even for canceled events, use listen_always.
 *
 * TODO: add handler priority classes.
 *
 * TODO: events may get "result" values in the future; if this happens we will
 *       need to parameterize on the return type also, not to mention give some
 *       thought to the way in which the results of multiple callbacks are
 *       aggregated.
 */
template<class EventType>
void listen(boost::function<void(EventType&)> callback)
{
    EventType::signal.connect(wrapper<EventType>(callback));
}

template<class EventType>
void listen(void (*callback)(EventType&))
{
    EventType::signal.connect(wrapper<EventType>(callback));
}


/******************************************************************************
 * Function: listen_always( callback )
 *
 * Registers an event listener function, as per listen, with the distinction
 * that the function will always be called when the appropriate event is
 * raised, even if the event gets canceled.
 */
template<class EventType>
void listen_always(boost::function<void(EventType&)> callback)
{
    EventType::signal.connect(callback);
}

template<class EventType>
void listen_always(void (*callback)(EventType&))
{
    EventType::signal.connect(callback);
}

/******************************************************************************
 * Function: fire
 *
 * Fires an event object, calling all listener functions that are registered to
 * the event.
 */
template<class EventType>
void fire(EventType& e)
{
    EventType::signal(e);
}

/******************************************************************************
 * Function: async_fire
 *
 * Fires an event asynchronously. This function returns immediately, and the
 * callbacks associated with the event are run as soon as possible.
 *
 * TODO: if events get return values, async_fire will need to take a callback
 *       to deliver the result.
 */
template<class EventType>
void async_fire(EventType& e)
{
    io_service().post(boost::bind(boost::ref(EventType::signal), e));
}

/******************************************************************************
 * Function: schedule
 *
 * Schedules a callback to be run in the specified number of milliseconds.
 *
 * WARNING: if the specified callback accesses any objects by reference or
 *  pointer, you must ensure that it is impossible for those objects to be
 *  destroyed before the interval expires. TODO: provide some way to cancel
 *  a pending timeout.
 */
void schedule(unsigned int ms, boost::function<void()> callback);

/******************************************************************************
 * Class: interval_timer
 *
 * Runs a callback at a specified regular interval, automatically canceling
 * the interval when the interval_timer is destroyed.
 */
class interval_timer
{
public:
    /**
     * Constructor parameters:
     *  ms      Duration of interval, in milliseconds
     *  cb      void() callback function to be called
     *
     * Remarks:
     *  The first invocation of the callback will be `ms` milliseconds after
     *  the call to the constructor. The callback will then be invoked every
     *  `ms` milliseconds thereafter until this object is destroyed. No calls
     *  will occur after the object is destroyed. 
     */
    interval_timer(unsigned int ms, std::function<void()> const& cb);
    ~interval_timer();

    interval_timer(interval_timer const&) = delete;
    void operator=(interval_timer const&) = delete;

private:
    unsigned int ms_;
    std::function<void()> cb_;
    boost::asio::deadline_timer timer_;

    void fire(boost::system::error_code const&);
};

} // end namespace boostcraft

