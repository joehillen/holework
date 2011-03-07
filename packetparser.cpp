// packetparser.cpp

#include "packetparser.h"
#include "packet.h"

#include <iostream>
#include <iomanip>
#include <vector>
#include <boost/system/error_code.hpp>
#include <boost/asio.hpp>

PacketParser::PacketParser()
  : buffer_(5)
{
}

boost::asio::streambuf& PacketParser::buffer()
{
  return buffer_;
}

size_t PacketParser::done(const boost::system::error_code& error, size_t bytes_read)
{
    if (error) {
        return 0;
    }

    if (buffer_.size() == 0)
        return buffer_.max_size();


    if ( !packet )
    {
        uint8_t id = boost::asio::buffer_cast<const uint8_t*>(buffer_.data())[0];
        buffer_.consume(1);

        std::pair<Packet::pointer, std::list<PacketField::pointer> > packet_pair = packetFactory(id);
        
        packet = packet_pair.first;
        fieldList = packet_pair.second;
    }

    if (fieldList.empty())
        return 0;
    
    // Read fields until we empty the buffer
    while(buffer_.size() > 0)
    {
        int need = fieldList.front()->readFrom(buffer_);

        // When a field is complete, remove it from the list
        if (need == 0)
        {
            std::cout << "Completed a field" << std::endl;
            fieldList.pop_front();

            // Done with packet when there are no more fields
            if (fieldList.empty())
                return 0;
        }
    }

    return buffer_.max_size();




  std::cout << "Parsing Data..." << std::endl;
  std::cout << "Bytes read: " << std::dec << buffer_.size()  << std::endl;

  const uint8_t* data = boost::asio::buffer_cast<const uint8_t*>(buffer_.data());
  for(size_t i = 0; i < buffer_.size(); ++i)
  {
     std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)data[i] << " ";
  }
  std::cout << std::endl;
  buffer_.consume(buffer_.size());
  return 50;
}
