
#include "packetparser.h"
#include "packet.h"
#include "types.h"

#include <iostream>
#include <iomanip>
#include <vector>
#include <boost/system/error_code.hpp>
#include <boost/asio.hpp>

PacketParser::PacketParser()
  : buffer_(5), position_(0)
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
