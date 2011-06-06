// main.cpp

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


#include <iostream>
#include <string>

#include <boost/asio.hpp>

#include <gtest/gtest.h>

#include "events.h"
#include "extras/pdl.h"
#include "handlers.h"
#include "log.h"
#include "server.h"

void test()
{
    using namespace boostcraft;
    log(INFO, "Timer","Timer expired!");
}

bool boostcraft::debug_mode = false;

int main(int argc, char** argv)
{
    using namespace boost::asio::ip;
    using namespace boostcraft;

    schedule(3000, test);

    for (int i = 1; i < argc; ++i)
    {
        std::string s(argv[i]);

        if (s == "--test")
        {
            ::testing::InitGoogleTest(&argc, argv);
            return RUN_ALL_TESTS();
        } 
        else if (s == "-d") 
        {
           debug_mode = true; 
           log(DEBUG, "main", "DEBUG Mode Enabled");
        }
    }

    // Initialize in-game packet debugging module
    boostcraft::pdl::init();

    listen(loginHandler);
    listen(logHandler);
    listen(lookHandler);
    listen(positionHandler);
    listen(ongroundHandler);

    tcp::endpoint endpoint(tcp::v4(), 25565);
    Server server(io_service(), endpoint);

    while(true)
    {
        try {
            io_service().run();
            break;
        }
        catch (std::exception& e) {
            std::cerr << e.what() << std::endl;
        }
    }
    return 0;
}

TEST(maintestcase, test1)
{
    ASSERT_TRUE(true);
}

