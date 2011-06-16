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

#include "event/types.h"
#include "ioservice.h"
#include "log.h"
#include "server.h"
#include "world.h"

/////////////////////////////////////////
// Add-on modules
#include "extras/pdl.h"
#include "extras/music.h"
#include "handlers/handlers.h"
#include "handlers/commands.h"


/////////////////////////////////////////
// io_service() implemented here

namespace boostcraft {
    boost::asio::io_service& io_service()
    {
        static boost::asio::io_service the_service;
        return the_service;
    }
}

bool boostcraft::debug_mode = false;


/////////////////////////////////////////
// The buck starts here

int main(int argc, char** argv)
{
    using namespace boost::asio::ip;
    using namespace boostcraft;
    using namespace boostcraft::event;

    // Process command line args
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

    // Initialize add-on modules
    boostcraft::pdl::init();
    boostcraft::stupidmusic::init();
    boostcraft::handlers::init();
    boostcraft::commands::init();

    // Start server
    tcp::endpoint endpoint(tcp::v4(), 25565);
    Server server(io_service(), endpoint);

    // Add worlds
    std::unique_ptr<World> world(new World(generator));
    server.addWorld(world);

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

// If this unit test fails, destroy universe
TEST(maintestcase, test1)
{
    ASSERT_TRUE(true);
}

