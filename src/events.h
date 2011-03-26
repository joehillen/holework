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

#include <boost/signals2/signal.hpp>

namespace boostcraft
{

// Forward declarations
class Player;
namespace network {
class LoginRequest;
}


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

    network::LoginRequest& request;

    LoginRequestEvent(Player& player, network::LoginRequest& request)
        : PlayerEvent(player), request(request)
    {
    }
};


// signal connection stuff


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


/**
 * Fires an event
 */
template<class EventType>
void fire(EventType& e)
{
    EventType::signal(e);
}



} // end namespace boostcraft
