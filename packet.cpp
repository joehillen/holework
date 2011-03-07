// packet.cpp

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
            ptr.reset(new LoginRequest);
            break;
        case PACKET_HANDSHAKE:
        {
            ClientHandshake* packet = new ClientHandshake();
            list.push_back(StringField::create(packet->username, 16));
            ptr.reset(packet);
            break;
        }
        case PACKET_CHAT_MESSAGE:
            ptr.reset(new ChatMessage);
            break;
        case PACKET_ENTITY_EQUIPMENT:
            ptr.reset(new EntityEquipment);
            break; 
        default:
            throw std::runtime_error("Unrecognized PacketID");
    }
    return std::pair<Packet::pointer, std::list<PacketField::pointer> >(ptr, list);
}


