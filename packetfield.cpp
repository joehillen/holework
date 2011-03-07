
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
