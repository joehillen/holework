// packet.cpp

/* These are methods used to generate server response packets.*/
sometype login(int playerID, long mapSeed, char dimension)
{
  sometype buffer;
  
  buffer << (uint8_t)PACKET_LOGIN_RESPONSE;
  buffer << (uint32_t)playerID;
  buffer << (uint16_t)0; // size of first string (unused)
  buffer << (uint16_t)0; // size of second string (unused)
  buffer << (uint64_t)mapSeed;
  buffer << (uint8_t)dimension;
  
  return buffer;
}



readbuf(PacketBuf& packetBuf)
{
  if (packetBuf == null)
  {
    char* packet_type;
    sync_read(socket, packet_type, 1)
    async_read(bind(readbuf, new PacketBuf(packet_type)))
  }
  else
  {
    if (!packetBuf.done())
      async_read(bind(readbuf, packetBuf))
  }
}
