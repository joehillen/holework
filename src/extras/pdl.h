// pdl.h
//
// Interface for Packet Description Language module
//
// Enable this module by calling xim::pdl::init() before starting the
// server; then player chat messages of a certain form will be intercepted and
// translated into packets which will be delivered directly to the player.
//
// This module is intended for extremely dangerous debugging purposes. :)
//
#pragma once

namespace xim {
namespace pdl {

    void init();

}} // namespace xim::pdl

