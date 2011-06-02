// events.cpp

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

#include "events.h"

#include <memory>


#include <iostream>
namespace boostcraft {

boost::asio::io_service the_service;

boost::asio::io_service& io_service()
{
    return the_service;
}


/**
 * Function: timeout_wrapper
 *
 * The only purpose of this function is to bind a deadline_timer to its
 * associated callback, so that the timer is not destroyed before it expires.
 */
void timeout_wrapper(std::shared_ptr<boost::asio::deadline_timer> timer,
        boost::function<void()> callback)
{
    callback();
}

void schedule(unsigned int ms, boost::function<void()> callback)
{
    std::shared_ptr<boost::asio::deadline_timer>
        timer(new boost::asio::deadline_timer(io_service()));

    timer->expires_from_now(boost::posix_time::milliseconds(ms));

    timer->async_wait(boost::bind(timeout_wrapper, timer, callback));
}


/******************************************************************************
 * interval_timer implementation
 */

interval_timer::interval_timer(
        unsigned int ms, std::function<void()> const& cb)
    : ms_(ms), cb_(cb), timer_(io_service())
{
    timer_.expires_from_now(boost::posix_time::milliseconds(ms_));
    timer_.async_wait(std::bind(&interval_timer::fire, this));
}

interval_timer::~interval_timer()
{
}

void interval_timer::fire()
{
    std::cout << "Firing interval_timer...\n";
    cb_();
    timer_.expires_from_now(boost::posix_time::milliseconds(ms_));
    timer_.async_wait(std::bind(&interval_timer::fire, this));
}



/******************************************************************************
 * Event class signal initialization
 *
 * TODO: might be able to remove all this manual initialization with
 *         some clever templating
 * TODO: move event types stuff to a different file, at any rate
 */
boost::signals2::signal<void(LoginRequestEvent&)> LoginRequestEvent::signal;
boost::signals2::signal<void(ChatEvent&)> ChatEvent::signal;
boost::signals2::signal<void(LogEvent&)> LogEvent::signal;
boost::signals2::signal<void(PlayerNeedsChunkEvent&)> PlayerNeedsChunkEvent::signal;
boost::signals2::signal<void(PlayerLookEvent&)> PlayerLookEvent::signal;
boost::signals2::signal<void(PlayerPositionEvent&)> PlayerPositionEvent::signal;
boost::signals2::signal<void(PlayerOnGroundEvent&)> PlayerOnGroundEvent::signal;

}

