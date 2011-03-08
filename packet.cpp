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

std::pair<Packet::pointer, std::list<PacketField::pointer> > packetFactory(int id)
{
    Packet::pointer ptr;
    std::list<PacketField::pointer> list;

    switch(id)
    {
        case PACKET_KEEP_ALIVE:
            ptr.reset(new KeepAlive);
            break;
        case PACKET_LOGIN_REQUEST:
            LoginRequest* packet = new LoginRequest();
            list.push_back(IntField::create(packet->protocolVersion));
            list.push_back(StringField::create(packet->username, MAX_USERNAME_LENGTH));
            list.push_back(StringField::create(packet->password));
            list.push_back(LongField::create(packet->mapSeed));
            list.push_back(ByteField::create(packet->dimension));
            ptr.reset(packet);
            break;
        case PACKET_HANDSHAKE:
            ClientHandshake* packet = new ClientHandshake();
            list.push_back(StringField::create(packet->username, MAX_USERNAME_LENGTH));
            ptr.reset(packet);
            break;
        case PACKET_CHAT_MESSAGE:
            ChatMessage* packet = new ChatMessage();
            list.push_back(StringField::create(packet->message));
            ptr.reset(packet);
            break;
        case PACKET_ENTITY_EQUIPMENT:
            EntityEquipment* packet = new EntityEquipment();
            list.push_back(StringField::create(packet->message));
            ptr.reset(packet);
            break;
        case PACKET_USE_ENTITY:
            UseEntity* packet = new UseEntity();
            list.push_back(StringField::create(packet->message));
            ptr.reset(packet);
            break;
        case PACKET_RESPAWN:
            ptr.reset(new Respawn);
            break;
        default:
            throw std::runtime_error("Unrecognized PacketID");
    }
    return std::pair<Packet::pointer, std::list<PacketField::pointer> >(ptr, list);
}


