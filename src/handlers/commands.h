// handlers.cpp

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

#include "handlers.h"

#include <boost/format.hpp>
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
#include <string>
#include <list>

#include "player.h"
#include "events.h"
#include "network/response.h"
#include "extras/music.h"

#define foreach BOOST_FOREACH

/**
 *
 * Commands is a ChatEvent listener for responsing to commands entered by users
 *
 *  TODO: A permission system for checking prevalages before running commands
 *
 **/

namespace boostcraft {

namespace handlers {

void commands(ChatEvent& e)
{
    using namespace boostcraft;
    using namespace boostcraft::network;
    std::shared_ptr<Player> p(e.player);

    if (e.message.length() < 1 or e.message[0] != '/')
    {
        return;
    }
    else
    {
        e.canceled = true;
    }

    // tokenize the message
    boost::tokenizer<> tok(e.message);
    std::queue<std::string> args;
    for(boost::tokenizer<>::iterator beg=tok.begin(); beg!=tok.end();++beg)
    {
        args.push(*beg);
    }

    if (args.size() < 1)
    {
        return;
    }

    std::string command(args.front());
    args.pop();

    if (command == "music")
    {
        using namespace stupidmusic;
        std::list<int> notes;
        std::list<int> times;

        if (args.size() < 1)
        {
            p->deliver(chatmessage("Argument required"));
            p->deliver(chatmessage("Try /music help"));
            return;
        }

        std::string arg = args.front();
        args.pop();

        if (arg == "major")
        {
            p->deliver(chatmessage("MUSIC TIME"));
            notes = { C4, D4, E4, F4, G4, A4, B4, C5, C5, B4, A4, G4, F4, E4, D4, C4 };
            times = {  Q,  Q,  Q,  Q,  Q,  Q,  Q,  Q,  Q,  Q,  Q,  Q,  Q,  Q,  Q,  Q };
        }
        else if (arg == "minor")
        {
            p->deliver(chatmessage("MUSIC TIME"));
            notes = { C4, D4, Ds4, F4, G4, Gs4, As4, C5, C5, As4, Gs4, G4, F4, Ds4, D4, C4 };
            times = {  Q,  Q,   Q,  Q,  Q,   Q,   Q,  Q,  Q,   Q,   Q,  Q,  Q,   Q,  Q,  Q };
        }
        else if (arg == "all")
        {
            p->deliver(chatmessage("MUSIC TIME"));
            notes = { Fs3,  G3,  Gs3,  A3,  As3,  B3, C4,  Cs4, D4, Ds4, E4, F4, Fs4,  G4,  Gs4,  A4,  As4,  B4, C5,  Cs5, D5, Ds5, E5, F5, Fs5 };
            times = { Q,    Q,   Q,    Q,   Q,    Q,  Q,   Q,   Q,  Q,   Q,  Q,  Q,    Q,   Q,    Q,   Q,    Q,  Q,   Q,   Q,  Q,   Q,  Q,  Q   };
        }
        else if (arg == "help")
        {
            std::list<std::string> command_list = {
                "/music major: play a minor scale",
                "/music minor: play a minor scale",
                "/music all: play all the notes"
            };

            foreach(std::string str, command_list)
            {
                p->deliver(chatmessage(str));
            }
        }
        else
        {
            p->deliver(chatmessage("Unrecognized argument"));
            p->deliver(chatmessage("Try /music help"));
        }
        playSequence(std::weak_ptr<Player>(p), notes, times);
    }
    else if (command == "help")
    {
        std::list<std::string> command_list = {
            "Available Commands:",
            "/music: play a little song"
        };

        foreach(std::string str, command_list)
        {
            p->deliver(chatmessage(str));
        }
    }
    else
    {
        p->deliver(chatmessage("Unrecognized command"));
        p->deliver(chatmessage("Try /help for a list of available commands."));
    }

}

}} //end namespace boostcraft::handlers

