// packetfield.cpp

#include "packetfield.h"
#include <gtest/gtest.h>

using namespace boostcraft::network;

TEST(PacketFieldTests, ShortField)
{
    boost::asio::streambuf b;
    std::ostream os(&b);
    os.put(0x10);
    os.put(0x80);

    uint16_t val = 0;

    ShortField field(val);

    field.readFrom(b);

    ASSERT_EQ(0x1080, val);
}

TEST(PacketFieldTests, DoubleField)
{
    boost::asio::streambuf b;
    std::ostream os(&b);
    
    // Double value 1337.1337 in network byte order:
    os.put(0x40);
    os.put(0x94);
    os.put(0xE4);
    os.put(0x88);
    os.put(0xE8);
    os.put(0xA7);
    os.put(0x1D);
    os.put(0xE7);

    double val = 0.0d;

    DoubleField field(val);
    field.readFrom(b);

    // This number should be exactly representable
    ASSERT_EQ(1337.1337, val);
}

TEST(PacketFieldTests, DoubleFieldContinuable)
{
    boost::asio::streambuf b;
    std::ostream os(&b);

    double val = 0.0d;

    DoubleField field(val);

    // All 8 bytes should still be required at first
    ASSERT_EQ(8, field.readFrom(b));

    // Double value 4.6855785559756050e-277
    // Put only the first three bytes in the buffer
    os.put(0x06);
    os.put(0x90);
    os.put(0x9c);

    // Should need five more bytes
    ASSERT_EQ(5, field.readFrom(b));

    os.put(0xa5);

    ASSERT_EQ(4, field.readFrom(b));

    os.put(0x45);
    os.put(0x8c);
    os.put(0xa9);
    os.put(0xc6);

    ASSERT_EQ(0, field.readFrom(b));

    // This value should be exactly represented so direct equality is OK
    ASSERT_EQ(4.6855785559756050e-277, val);
}






