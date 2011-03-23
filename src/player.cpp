// player.h

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
#include "connection.h"
#include "inventory.h"
#include "item.h"

#define MAX_DELTA 10000 //change this or make it configurable?

class Player
{
public:
    Player()
    {
        
    }
    
    Player(Connection::pointer connection)
    {
        connection_ = connection;
    }
    
    void connection(Connection::pointer connection) 
    {
        connection_ = connection;
    }
    
    Connection::pointer connection()
    {
        return connection_;
    }
    
    void handle(Packet& packet)
    {
        switch(packet->type)
        {
            case PACKET_LOGIN:
            {
                if (packet->name)
                {
                    name_ = packet->name;
                }
            }
            default:
                throw std::read_error("Unrecognized packet");
        }
    }
    
    void updatePosition(double x, double y, double z)
    {
        x_ = x;
        x_ = y;
        x_ = z;
        /* TODO: Something that tells everyone the player position changed. */
    }
    
    void updateLook(float yaw, float pitch)
    {
        yaw_ = yaw;
        pitch_ = pitch;
        /* TODO: Something that tells everyone the player look changed. */
    }
    
    void updateInventory(Item id)
    {
        
    }
    
    void updateHealth(short health)
    {
        if (health >= 20)
        {
            health_ = 20;
        }
        else if (health <= 0)
        {
            health = 0;
            kill();
        }
        else
        {
            health_ = health;
        }
    }
    
    void kill()
    {
        
    }
    
private:
    int id_;
    
    std::string name_;

    /* Position */
    double x_;
    double z_;
    double y_;

    /* Look */
    float yaw_;
    float pitch_;

    /* State */
    bool on_ground_;
    bool digging_;
    short health_;

    /* Spawn */
    double spawn_x_;
    double spawn_z_;
    double spawn_y_;
    
    Item holding_;
    Inventory inv_;
    
    Connection::pointer connection_;
};
