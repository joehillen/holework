// handlers/commands.cpp
//
#include "handlers/commands.h"

#include <boost/format.hpp>
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
#include <string>
#include <list>

#include "player.h"
#include "event/types.h"
#include "network/response.h"
#include "extras/music.h"

#define foreach BOOST_FOREACH

namespace boostcraft {
namespace commands {

using namespace boostcraft::event;
using namespace boostcraft::network;

void commands(ChatEvent& e)
{
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

void init()
{
    event::listen(commands);
}

}} // namespace boostcraft::commands

