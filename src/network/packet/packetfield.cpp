// packetfield.cpp

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

#include "packetfield.h"

#include <iostream>


int main()
{
    boost::asio::streambuf buf(500);
    std::ostream os(&buf);

    buf.prepare(12);

    os << char(0);
    os << char(20);
    os << "Hello, wor";


/*    std::cout << buf.size() << std::endl;

    const char* data = boost::asio::buffer_cast<const char*>(buf.data());
    for (int i = 0; i < buf.size(); ++i)
        std::cout << data[i];

    std::cout << std::endl;

    buf.consume(buf.size());

    std::cout << buf.size() << std::endl;
*/

    std::string test;

    StringField field(test);

    int need;

    do {
        need = field.readFrom(buf);
        std::cout << "Still need " << need << " bytes." << std::endl;

        os << "fwds";
    } while(need);

    std::cout << "Needs: " << need << std::endl << "String: " << test << std::endl;

    return 0;
}
