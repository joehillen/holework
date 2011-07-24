// event/types.h
// Definition of event types

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
#pragma once

#include "event/event.h"
#include "log.h"
#include "position.h"
#include "player.h"

/// Forward declarations
namespace boostcraft {
    class World;
    class Chunk;
}

namespace boostcraft {
namespace event {

struct PlayerEvent : public Event
{
    player_ptr player;
    std::shared_ptr<World> world;

    PlayerEvent(std::shared_ptr<Player> player) 
        : player(player), world(player->world().lock()) 
    { }
};

struct LogEvent : public Event, Signal<LogEvent>
{
    std::string message;
    LogType type;

    LogEvent(boostcraft::LogType type, std::string const& msg) : message(msg), type(type)
    {}
};

struct LoginRequestEvent : public PlayerEvent, Signal<LoginRequestEvent>
{
    int version;
    std::string username;

    LoginRequestEvent(std::shared_ptr<Player> player,
            std::string const& username, int version)
        : PlayerEvent(player), version(version), username(username)
    {
    }
};

struct ChatEvent : public PlayerEvent, Signal<ChatEvent>
{
    std::string message;

    ChatEvent(std::shared_ptr<Player> player, std::string const& msg)
        : PlayerEvent(player), message(msg)
    {
    }
};

struct NeedChunkEvent : public Event, Signal<NeedChunkEvent>
{
    std::shared_ptr<World> world;
    int x;
    int z;

    NeedChunkEvent(std::shared_ptr<World> world, int x, int z)
        : world(world), x(x), z(z)
    {
    }
};

struct NewChunkEvent : public Event, Signal<NewChunkEvent>
{
    std::shared_ptr<World> world;
    int x;
    int z;
    std::shared_ptr<Chunk> chunk;

    NewChunkEvent(std::shared_ptr<World> world, int x, int z, std::shared_ptr<Chunk> chunk)
        : world(world), x(x), z(z), chunk(chunk)
    {
    }
};

struct PlayerLookEvent : public PlayerEvent, Signal<PlayerLookEvent>
{
    float yaw;
    float pitch;

    PlayerLookEvent(std::shared_ptr<Player> player, float yaw, float pitch)
        : PlayerEvent(player), yaw(yaw), pitch(pitch)
    {
    }
};

struct PlayerPositionEvent : public PlayerEvent, Signal<PlayerPositionEvent>
{
    EntityPosition position;

    PlayerPositionEvent(std::shared_ptr<Player> player, EntityPosition position)
        : PlayerEvent(player), position(position)
    {
    }
};

struct PlayerOnGroundEvent : public PlayerEvent, Signal<PlayerOnGroundEvent>
{
    bool on_ground;

    PlayerOnGroundEvent(std::shared_ptr<Player> player, bool on_ground)
        : PlayerEvent(player), on_ground(on_ground)
    {
    }
};

struct PlayerDisconnectEvent : public PlayerEvent, Signal<PlayerDisconnectEvent>
{
    std::string reason;

    PlayerDisconnectEvent(std::shared_ptr<Player> player,
            std::string const& reason)
        : PlayerEvent(player), reason(reason)
    {
    }
};

}} // namespace boostcraft::event

