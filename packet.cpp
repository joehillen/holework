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


