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
#include <vector>

#include <boost/asio.hpp>
#include <boost/format.hpp>

#include <gtest/gtest.h>

#include "log.h"
#include "events.h"
#include "network/tcpserver.h"  
#include "chunk.h"
#include "handlers.h"

using namespace boostcraft;

int main(int argc, char** argv)
{
    using namespace boost::asio::ip;
    using namespace boostcraft::network;

    for (int i = 1; i < argc; ++i)
    {
        std::string s(argv[i]);

        if (s == "--test")
        {
            ::testing::InitGoogleTest(&argc, argv);
            return RUN_ALL_TESTS();
        }
    }

    listen(loginHandler);
    listen(logHandler);
    listen(lookHandler);
    listen(positionHandler);
    listen(ongroundHandler);

    try
    {
        tcp::endpoint endpoint(tcp::v4(), 25565);
        TcpServer server(io_service(), endpoint);

        io_service().run();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}

TEST(maintestcase, test1)
{
    ASSERT_TRUE(false);
}

