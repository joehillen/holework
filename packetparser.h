// packetparser.h

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
#include "packetfield.h"

#include <list>
#include <boost/system/error_code.hpp>
#include <boost/asio.hpp>

#define DEFAULT_MAXLEN 100

/*
 * PacketParser facilitates the reading of Minecraft packets with boost::asio
 * read() or async_read(). A PacketParser's buffer and completion function can
 * be provided in the asio read call, and then the read handler will be called
 * as soon as the PacketParser has parsed one complete Minecraft packet.
 */
class PacketParser
{
public:
    PacketParser();

    boost::asio::streambuf& buffer();

    size_t done(const boost::system::error_code& error, size_t bytes_read);

private:
    void parse();

    boost::asio::streambuf buffer_;

    Packet::pointer packet;
    std::list<PacketField::pointer> fieldList;
};
