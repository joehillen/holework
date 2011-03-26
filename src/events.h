// events.h

#pragma once

#include <boost/signals2/signal.hpp>

namespace boostcraft
{

// Forward declarations
class Player;
class LoginRequest;


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
    Player& player;

    PlayerEvent(Player& player) : player(player) { }
};

struct LogEvent : public Event
{
    static boost::signals2::signal<void(LogEvent&)> signal;
    std::string message;

    LogEvent(std::string const& msg) : message(msg) { }
};




struct LoginRequestEvent : public PlayerEvent
{
    static boost::signals2::signal<void(LoginRequestEvent&)> signal;

    LoginRequest& request;

    LoginRequestEvent(Player& player, LoginRequest& request)
        : PlayerEvent(player), request(request)
    {
    }
};


// Connection stuff


template<class EventType>
void callIfNotCanceled(
        boost::function<void(EventType&)> callback,
        EventType& event)
{
    if(!event.canceled)
        callback(event);
}

/**
 * Wraps an event signal callback so that it will not be called if the event
 * that dispatched it was cancelled by a prior signal callback.
 */
template<class EventType>
boost::function<void(EventType&)> wrapper(
        boost::function<void(EventType&)> callback)
{
    return boost::bind(callIfNotCanceled<EventType>, callback, _1);
}


template<class EventType>
void listen(boost::function<void(EventType&)> callback)
{
    EventType::signal.connect(wrapper<EventType>(callback));
}



} // end namespace boostcraft
