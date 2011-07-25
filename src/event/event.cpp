// event/event.cpp

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

#include "event/event.h"

#include <functional>
#include <memory>

namespace xim {
namespace event {

/**
 * Function: timeout_wrapper
 *
 * The only purpose of this function is to bind a deadline_timer to its
 * associated callback, so that the timer is not destroyed before it expires.
 */
void timeout_wrapper(std::shared_ptr<boost::asio::deadline_timer> timer,
        std::function<void()> callback)
{
    callback();
}

void schedule(unsigned int ms, std::function<void()> callback)
{
    std::shared_ptr<boost::asio::deadline_timer>
        timer(new boost::asio::deadline_timer(io_service()));

    timer->expires_from_now(boost::posix_time::milliseconds(ms));

    timer->async_wait(std::bind(timeout_wrapper, timer, callback));
}


/******************************************************************************
 * interval_timer implementation
 */

interval_timer::interval_timer(
        unsigned int ms, std::function<void()> const& cb)
    : ms_(ms), cb_(cb), timer_(io_service())
{
    timer_.expires_from_now(boost::posix_time::milliseconds(ms_));
    timer_.async_wait(boost::bind(
                &interval_timer::fire, this,
                boost::asio::placeholders::error));
}

interval_timer::~interval_timer()
{
    timer_.cancel();
}

void interval_timer::fire(boost::system::error_code const& error)
{
    if(!error)
    {
        cb_();
        timer_.expires_from_now(boost::posix_time::milliseconds(ms_));
        timer_.async_wait(boost::bind(
                    &interval_timer::fire, this,
                    boost::asio::placeholders::error));
    }
}

}} // namespace xim::event

