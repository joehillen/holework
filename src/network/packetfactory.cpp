// packet.cpp

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


#include "packet.h"
#include "../log.h"

#include <sstream>
#include <iomanip>

namespace boostcraft { namespace network { 

std::pair<Request::pointer, std::list<PacketField::pointer> > 
packetFactory(int id)
{
    Request::pointer ptr;
    std::list<PacketField::pointer> list;

    switch(id)
    {
        case REQUEST_KEEP_ALIVE:
        {
            ptr.reset(new KeepAliveRequest);
            break;
        }
        case REQUEST_LOGIN:
        {
            LoginRequest* packet = new LoginRequest();
            list.push_back(IntField::create(packet->protocolVersion));
            list.push_back(String16Field::create(packet->username, MAX_USERNAME_LENGTH));
            list.push_back(LongField::create(packet->mapSeed));
            list.push_back(ByteField::create(packet->dimension));
            ptr.reset(packet);
            break;
        }
        case REQUEST_HANDSHAKE:
        {
            HandshakeRequest* packet = new HandshakeRequest();
            list.push_back(String16Field::create(packet->username, MAX_USERNAME_LENGTH));
            ptr.reset(packet);
            break;
        }
        case REQUEST_CHAT:
        {
            ChatRequest* packet = new ChatRequest();
            list.push_back(String16Field::create(packet->message));
            ptr.reset(packet);
            break;
        }
        case REQUEST_PLAYER:
        {
            PlayerRequest* packet = new PlayerRequest();
            list.push_back(BoolField::create(packet->on_ground));
            ptr.reset(packet);
        }
        case REQUEST_POSITION:
        {
            PositionRequest* packet = new PositionRequest();
            list.push_back(DoubleField::create(packet->x));
            list.push_back(DoubleField::create(packet->y));
            list.push_back(DoubleField::create(packet->stance));
            list.push_back(DoubleField::create(packet->z));
            list.push_back(BoolField::create(packet->on_ground));
            ptr.reset(packet);
            break;
        }
        case REQUEST_POSITION_AND_LOOK:
        {
            PositionLookRequest* packet = new PositionLookRequest();
            list.push_back(DoubleField::create(packet->x));
            list.push_back(DoubleField::create(packet->stance));
            list.push_back(DoubleField::create(packet->y));
            list.push_back(DoubleField::create(packet->z));
            list.push_back(FloatField::create(packet->yaw));
            list.push_back(FloatField::create(packet->pitch));
            list.push_back(BoolField::create(packet->on_ground));
            ptr.reset(packet);
            break;
        }
        /* TODO: FIX
        case REQUEST_ENTITY_EQUIPMENT:
        {
            EntityEquipment* packet = new EntityEquipment();
            list.push_back(IntField::create(packet->entity));
            list.push_back(ShortField::create(packet->slot));
            list.push_back(ShortField::create(packet->item));
            list.push_back(ShortField::create(packet->damage));
            ptr.reset(packet);
            break;
        }
        case REQUEST_USE_ENTITY:
        {
            UseEntity* packet = new UseEntity();
            list.push_back(IntField::create(packet->user));
            list.push_back(IntField::create(packet->target));
            list.push_back(BoolField::create(packet->left_click));
            ptr.reset(packet);
            break;
        }
        case REQUEST_RESPAWN:
        {
            ptr.reset(new Respawn);
            break;
        }
        case REQUEST_PLAYER:
        {
            Player* packet = new Player();
            list.push_back(BoolField::create(packet->on_ground));
            ptr.reset(packet);
            break;
        }
        case REQUEST_LOOK:
        {
            Look* packet = new Look();
            list.push_back(FloatField::create(packet->yaw));
            list.push_back(FloatField::create(packet->pitch));
            list.push_back(BoolField::create(packet->on_ground));
            ptr.reset(packet);
            break;
        }
        case REQUEST_DIG:
        {
            Dig* packet = new Dig();
            list.push_back(ByteField::create(packet->status));
            list.push_back(IntField::create(packet->x));
            list.push_back(ByteField::create(packet->y));
            list.push_back(IntField::create(packet->z));
            list.push_back(ByteField::create(packet->face));
            ptr.reset(packet);
            break;
        }
        case REQUEST_PLACE_BLOCK:
        {
            PlaceBlock* packet = new PlaceBlock();
            list.push_back(IntField::create(packet->x));
            list.push_back(ByteField::create(packet->y));
            list.push_back(IntField::create(packet->z));
            list.push_back(ByteField::create(packet->direction));
            list.push_back(ShortField::create(packet->block));
            // We need a mechanic for handling optional fields
            // list.push_back(ByteField::create(packet->amount));
            // list.push_back(ShortField::create(packet->damage));
            
            ptr.reset(packet);
            break;
        }
        case REQUEST_HOLD_CHANGE:
        {
            HoldChange* packet = new HoldChange();
            list.push_back(ShortField::create(packet->slot));
            ptr.reset(packet);
            break;
        }
        case REQUEST_ANIMATION:
        {
            Animation* packet = new Animation();
            list.push_back(IntField::create(packet->player));
            list.push_back(ByteField::create(packet->animation));
            ptr.reset(packet);
        }
        case REQUEST_CROUCH:
        {
            Crouch* packet = new Crouch();
            list.push_back(IntField::create(packet->player));
            list.push_back(ByteField::create(packet->crouch));
            ptr.reset(packet);
        }
        case REQUEST_INVENTORY_CLOSE:
        {
            InvClose* packet = new InvClose();
            list.push_back(ByteField::create(packet->window));
            ptr.reset(packet);
        }
        case REQUEST_INVENTORY_CHANGE:
        {
            InvChange* packet = new InvChange();
            list.push_back(ByteField::create(packet->window));
            list.push_back(ShortField::create(packet->slot));
            list.push_back(BoolField::create(packet->right_click));
            list.push_back(ShortField::create(packet->action));
            list.push_back(ShortField::create(packet->item));
            list.push_back(ByteField::create(packet->count));
            list.push_back(ShortField::create(packet->uses));
            ptr.reset(packet);
        }
        case REQUEST_SIGN:
        {
            Sign* packet = new Sign();
            list.push_back(IntField::create(packet->x));
            list.push_back(ShortField::create(packet->y));
            list.push_back(IntField::create(packet->z));
            list.push_back(String16Field::create(packet->text1));
            list.push_back(String16Field::create(packet->text2));
            list.push_back(String16Field::create(packet->text3));
            list.push_back(String16Field::create(packet->text4));
        }*/
        default:
            std::stringstream ss;
            ss << "Unrecognized PacketID: 0x" 
                << std::setfill ('0') << std::setw(2) << std::hex << id; 
            log(ERROR,"PacketFactory", ss.str());
            throw std::runtime_error("Unrecognized PacketID");
    }
    
    return std::pair<Request::pointer, std::list<PacketField::pointer> >(ptr, list);
}

}} //end namespace boostcraft::network

