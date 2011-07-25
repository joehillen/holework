// music.h
//
// Something stupid I felt like writing for no reason!
//
#pragma once

#include <list>
#include <memory>

// Forward declarations
namespace xim {
    class Player;
}

namespace xim {
namespace stupidmusic {

    void init();

    enum duration {
        E = 250,
        Q = 500,
        H = 1000,
        W = 2000
    };

    enum pitches {
                                   Fs3,  G3,  Gs3,  A3,  As3,  B3,
        C4,  Cs4, D4, Ds4, E4, F4, Fs4,  G4,  Gs4,  A4,  As4,  B4,
        C5,  Cs5, D5, Ds5, E5, F5, Fs5
    };

    void playSequence(std::weak_ptr<Player> weakp,
            std::list<int> notes, std::list<int> times);

}} // namespace xim::stupidmusic

