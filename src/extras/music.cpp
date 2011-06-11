// music.cpp
//
#include "extras/music.h"

#include "event/types.h"
#include "network/response.h"
#include "player.h"

#include <list>
#include <memory>

namespace boostcraft {
namespace stupidmusic {


void send_note(Player& p, int instrument, int pitch)
{
    network::Response r;
    r << (int8_t)0x36;
    r << (int32_t)0 << (int16_t)1 << (int32_t)0;
    r << (int8_t)instrument << (int8_t)pitch;
    p.deliver(r);
}

void playSequence(std::weak_ptr<Player> weakp,
        std::list<int> notes, std::list<int> times)
{
    //
    // Atempt to lock weak_ptr before doing anything; this ensures that we'll
    // stop playing music automatically when the target player disconnects.
    //
    auto player = weakp.lock();

    if (player && !notes.empty() && !times.empty())
    {
        int pitch = notes.front();
        int duration = times.front();

        // Play the head note and then schedule another call to this function
        send_note(*player, 0, pitch);
        notes.pop_front();
        times.pop_front();
        event::schedule(duration, std::bind(&playSequence, weakp, notes, times));
    }
}

void chatHandler(event::ChatEvent& e)
{
    if (e.message == "!music")
    {
        e.player->deliver(network::chatmessage("MUSIC TIME"));

        std::list<int> notes { Fs3,  G3,  Gs3,  A3,  As3, B3,  C4,  Cs4, D4, Ds4,
            E4, F4, Fs4,  G4,  Gs4,  A4,  As4,  B4,  C5,  Cs5, D5,  Ds5, E5, F5,  Fs5 };
        std::list<int> times { Q,    Q,   Q,    Q,   Q,   Q,   Q,   Q,   Q,  Q,   
            Q,  Q,  Q,    Q,   Q,    Q,   Q,    Q,   Q,   Q,   Q,   Q,   Q,  Q,   Q   };
        playSequence(std::weak_ptr<Player>(e.player), notes, times);

        e.canceled = true;
    }
}

void init()
{
    event::listen(chatHandler);
}

}} // namespace boostcraft::stupidmusic
