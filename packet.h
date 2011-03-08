// packet.h

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

#include "packetfield.h"

#define MAX_USERNAME_LENGTH 16

//Packet names
enum
{
  //Client to server
  PACKET_KEEP_ALIVE                = 0x00,
  PACKET_LOGIN_REQUEST             = 0x01,
  PACKET_HANDSHAKE                 = 0x02,
  PACKET_CHAT_MESSAGE              = 0x03,
  PACKET_ENTITY_EQUIPMENT          = 0x05,
  PACKET_RESPAWN                   = 0x09,
  PACKET_PLAYER                    = 0x0a,
  PACKET_POSITION                  = 0x0b,
  PACKET_LOOK                      = 0x0c,
  PACKET_POSITION_AND_LOOK         = 0x0d,
  PACKET_DIG                       = 0x0e,
  PACKET_PLACE_BLOCK               = 0x0f,
  PACKET_HOLD_CHANGE               = 0x10,
  PACKET_ANIMATION                 = 0x12,
  PACKET_CROUCH                    = 0x13,
  PACKET_INVENTORY_CLOSE           = 0x65,
  PACKET_INVENTORY_CHANGE          = 0x66,
  PACKET_SET_SLOT                  = 0x67,
  PACKET_INVENTORY                 = 0x68,
  PACKET_SIGN                      = 0x82,
  PACKET_DISCONNECT                = 0xff,
  //Server to client
  PACKET_LOGIN_RESPONSE            = 0x01,
  PACKET_TIME_UPDATE               = 0x04,
  PACKET_SPAWN_POSITION            = 0x06,
  PACKET_UPDATE_HEALTH             = 0x08,
  PACKET_ADD_TO_INVENTORY          = 0x11,
  PACKET_NAMED_ENTITY_SPAWN        = 0x14,
  PACKET_PICKUP_SPAWN              = 0x15,
  PACKET_COLLECT_ITEM              = 0x16,
  PACKET_ADD_OBJECT                = 0x17,
  PACKET_MOB_SPAWN                 = 0x18,
  PACKET_DESTROY_ENTITY            = 0x1d,
  PACKET_ENTITY                    = 0x1e,
  PACKET_ENTITY_RELATIVE_MOVE      = 0x1f,
  PACKET_ENTITY_LOOK               = 0x20,
  PACKET_ENTITY_LOOK_RELATIVE_MOVE = 0x21,
  PACKET_ENTITY_TELEPORT           = 0x22,
  PACKET_DEATH_ANIMATION           = 0x26,
  PACKET_PRE_CHUNK                 = 0x32,
  PACKET_MAP_CHUNK                 = 0x33,
  PACKET_MULTI_BLOCK_CHANGE        = 0x34,
  PACKET_BLOCK_CHANGE              = 0x35,
  PACKET_PLAY_NOTE                 = 0x36,
  PACKET_OPEN_WINDOW               = 0x64,
  PACKET_PROGRESS_BAR              = 0x69,
  PACKET_TRANSACTION               = 0x6a,
  //PACKET_COMPLEX_ENTITIES          = 0x3b,
  PACKET_KICK                      = 0xff,


  //v4 Packets
  PACKET_USE_ENTITY      = 0x07,
  PACKET_ENTITY_VELOCITY = 0x1c,
  PACKET_ATTACH_ENTITY   = 0x27
};

struct Packet
{
    typedef boost::shared_ptr<Packet> pointer;

    uint8_t type;
    
    Packet(int id) : type(id) { }
};

/**********************************
*      CLIENT ONLY PACKETS
**********************************/
struct KeepAlive : public Packet
{
    KeepAlive() : Packet(PACKET_KEEP_ALIVE) { }
};

struct ClientHandshake : public Packet
{
    ClientHandshake() : Packet(PACKET_HANDSHAKE) { }

    std::string username;
};

struct LoginRequest : public Packet
{
    LoginRequest() : Packet(PACKET_LOGIN_REQUEST) { }

    uint32_t protocolVersion;
    std::string username;
    std::string password;
    uint64_t mapSeed;
    uint8_t dimension;
};

struct ChatMessage : public Packet
{
    ChatMessage() : Packet(PACKET_CHAT_MESSAGE) { }
    std::string message;
};

struct EntityEquipment : public Packet
{
    EntityEquipment() : Packet(PACKET_ENTITY_EQUIPMENT) { }
    uint32_t entity;
    uint16_t slot;
    uint16_t item;
    uint16_t damage; //Damage??
};

struct UseEntity : public Packet
{
    UseEntity() : Packet(PACKET_USE_ENTITY) { }
    uint32_t user; //ignored by server
    uint16_t target; //entity the player is interacting with
    uint8_t left_click; // boolean, true is user left clicks, false otherwise
};

struct Respawn : public Packet
{
    Respawn() : Packet(PACKET_RESPAWN) { }
};

struct Player : public Packet
{
    Player() : Packet(PACKET_PLAYER) { }
    uint8_t on_ground; // boolean, True if the client is on the ground, False otherwise
};

struct Position : public Packet
{
    Position() : Packet(PACKET_POSITION) { }
    double x; //TODO: These doubles might need to be a different type
    double y;
    double stance;
    double z;
    uint8_t on_ground; // boolean, True if the client is on the ground, False otherwise
};

struct Look : public Packet
{
    Look() : Packet(PACKET_LOOK) { }
    float yaw; // TODO: These floats might need to be a different type 
    float pitch;
    uint8_t on_ground;
};

struct PositionLook : public Packet
{
    PositionLook() : Packet(PACKET_POSITION_AND_LOOK) { }
    double x; // TODO: These doubles might need to be a different type
    double stance;
    double y;
    double z;
    float yaw; // TODO: These floats might need to be a different type 
    float pitch;
    uint8_t on_ground;
};

// http://mc.kev009.com/Protocol#Player_Digging_.280x0E.29
struct Dig : public Packet
{
    Dig() : Packet(PACKET_DIG) { }
    uint8_t status; //byte
    uint32_t x; //int
    uint8_t y; //byte
    uint32_t z; //int
    uint8_t face; //byte
};

// http://mc.kev009.com/Protocol#Player_Block_Placement_.280x0F.29
struct PlaceBlock : Packet
{
    PlaceBlock() : Packet(PACKET_PLACE_BLOCK) { }
    uint32_t x; //int
    uint8_t y; //byte
    uint32_t z; //int
    uint8_t direction; //byte
    uint16_t block; //short
    uint8_t amount; //byte, optional
    uint16_t damage; //short, optional
};

// http://mc.kev009.com/Protocol#Holding_Change_.280x10.29
struct HoldChange : Packet
{
    HoldChange() : Packet(PACKET_HOLD_CHANGE) { }
    uint16_t slot; //short
};

// http://mc.kev009.com/Protocol#Animation_.280x12.29
struct Animation : Packet
{
    Animation() : Packet(PACKET_ANIMATION) { }
    uint32_t player; // player id
    uint8_t animation; 
};

// http://mc.kev009.com/Protocol#Entity_Action_.3F.3F.3F_.280x13.29
struct Crouch : Packet
{
    Crouch() : Packet(PACKET_CROUCH) { }
    uint32_t player; // player id
    uint8_t crouch; 
};

// http://mc.kev009.com/Protocol#Close_window_.280x65.29
struct InvClose : Packet
{
    InvClose() : Packet(PACKET_INVENTORY_CLOSE) { }
    uint8_t window; //byte 
};

// http://mc.kev009.com/Protocol#Window_click_.280x66.29
struct InvChange : Packet
{
    InvChange() : Packet(PACKET_INVENTORY_CHANGE) { }
    uint8_t window; // byte 
    uint16_t slot; // short 
    uint8_t right_click; // boolean
    uint16_t action; // short 
    uint16_t item; // short 
    uint8_t count; // byte
    uint16_t uses; // short
};


struct Sign : Packet
{
    Sign() : Packet(PACKET_SIGN) { }
    uint32_t x; // int
    uint16_t y; // short
    uint32_t z; // int
    std::string text1;
    std::string text2;
    std::string text3;
    std::string text4;
};


/**********************************
*      SERVER ONLY PACKETS
**********************************/



std::pair<Packet::pointer, std::list<PacketField::pointer> > packetFactory(int id);

