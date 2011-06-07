// inetconv.cpp

#include "inetconv.h"
#include <gtest/gtest.h>
#include <sys/types.h>

bool isbigendian()
{
    int n = 0x00FF;
    return *(char *)&n == 0;
}


/*
 * UNIT TESTS
 */

/*
 * It goes without saying, perhaps, but ntoh should never change the value of a
 * single byte.
 */
TEST(InetconvTests, NtohByte)
{
    for(int i = 0; i < 256; ++i)
        ASSERT_EQ(i, ntoh((uint8_t)i));
}

TEST(InetconvTests, NtohShort)
{
    ASSERT_EQ(0, ntoh((uint16_t)0));
    ASSERT_EQ(0xFFFF, ntoh((uint16_t)0xFFFF));

    if (isbigendian())
    {
        ASSERT_EQ(0x2010, ntoh((uint16_t)0x2010));
        ASSERT_EQ(0xFF7F, ntoh((uint16_t)0xFFF7));
    }
    else
    {
        ASSERT_EQ(0x1020, ntoh((uint16_t)0x2010));
        ASSERT_EQ(0x7FFF, ntoh((uint16_t)0xFF7F));
    }
}

TEST(InetconvTests, NtohInt)
{
    ASSERT_EQ(0, ntoh(0));
    ASSERT_EQ(0xFFFFFFFFU, ntoh(0xFFFFFFFFU));

    if (isbigendian())
    {
        ASSERT_EQ(0x10203040, ntoh(0x10203040));
        ASSERT_EQ(0x10A0C010, ntoh(0x10A0C010));
        ASSERT_EQ(0xFFFFFF7F, ntoh(0xFFFFFF7F));
    }
    else
    {
        ASSERT_EQ(0x40302010, ntoh(0x10203040));
        ASSERT_EQ(0x10C0A010, ntoh(0x10A0C010));
        ASSERT_EQ(0x7FFFFFFF, ntoh(0xFFFFFF7F));
    }
}

TEST(InetconvTests, NtohLong)
{
    ASSERT_EQ(0LLU, ntoh(0LLU));
    ASSERT_EQ(0xFFFFFFFFFFFFFFFFLLU, ntoh(0xFFFFFFFFFFFFFFFFLLU));

    if (isbigendian())
    {
        ASSERT_EQ(0x1020304050607080LLU, ntoh(0x1020304050607080LLU));
        ASSERT_EQ(0xC0C0FFEEDDCCC0C0LLU, ntoh(0xC0C0FFEEDDCCC0C0LLU));
    }
    else
    {
        ASSERT_EQ(0x8070605040302010LLU, ntoh(0x1020304050607080LLU));
        ASSERT_EQ(0xC0C0CCDDEEFFC0C0LLU, ntoh(0xC0C0FFEEDDCCC0C0LLU));
    }
}

/*
 * Verify that swapping the bytes of signed values works as expected.
 */
TEST(InetconvTests, NtohSigned)
{
    ASSERT_EQ(-1, ntoh((int16_t)-1));

    if (isbigendian())
    {
        ASSERT_EQ(1028323, ntoh(1028323));
        ASSERT_EQ(-5402, ntoh(-5402));
    }
    else
    {
        ASSERT_EQ((signed)0x10203040, ntoh((signed)0x40302010));
        ASSERT_EQ((signed)0xFF4080FF, ntoh((signed)0xFF8040FF));
    }       
}

TEST(InetconvTests, NtohDouble)
{
    double value = 5.291593411e+233d;
    // NOTE: this isn't a "real" double value; we're treating it as a double in
    // network byte order, so the true value is different from above. The hex
    // representation in network byte order is 0x70754D6E32C48F67.

    double expected = 0;

    if (isbigendian())
        expected = value;
    else
    {
        // Modify through char* to exploit strict aliasing exception
        unsigned char cexp[] = { 0x70, 0x75, 0x4D, 0x6E, 0x32, 0xC4, 0x8F, 0x67 };
        char* pexp = reinterpret_cast<char*>(&expected);
        for(int i = 0; i < 8; ++i) pexp[i] = cexp[i];
    }

    ASSERT_EQ(expected, ntoh(value));
}






