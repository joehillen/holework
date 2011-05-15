// inetconv.cpp

#include "inetconv.h"
#include <gtest/gtest.h>



bool isbigendian()
{
    int n = 0x00FF;
    return *(char *)&n == 0;
}


/**
 * Converts an unsigned 64-bit integer from network to host byte order.
 */
uint64_t ntohll(uint64_t n)
{
    if (isbigendian())
        return n;

    return (n>>56) | 
        ((n<<40) & 0x00FF000000000000LLU) |
        ((n<<24) & 0x0000FF0000000000LLU) |
        ((n<<8)  & 0x000000FF00000000LLU) |
        ((n>>8)  & 0x00000000FF000000LLU) |
        ((n>>24) & 0x0000000000FF0000LLU) |
        ((n>>40) & 0x000000000000FF00LLU) |
        (n<<56);
}

TEST(InetconvTests, TestNtohll)
{
    uint64_t x = 0x1122334455667788LLU;
    uint64_t expected;

    if (isbigendian())
        expected = x;
    else
        expected = 0x8877665544332211;

    ASSERT_EQ(expected, ntohll(x));
}



TEST(InetconvTests, TestNtohTemplateShort)
{
    uint16_t x = 0xAABB;
    uint16_t expected;

    if (isbigendian())
        expected = x;
    else
        expected = 0xBBAA;

    ASSERT_EQ(expected, ntoh(x));
}

TEST(InetconvTests, TestNtohDouble)
{
    // TODO: write a good test case
    bool test_case_is_good = false;
    ASSERT_TRUE(test_case_is_good);
}
