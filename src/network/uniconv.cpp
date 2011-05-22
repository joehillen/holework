// uniconv.cpp
// Functions for converting between different unicode string formats.
//

#include "uniconv.h"
#include "inetconv.h"

#include <iconv.h>
#include <gtest/gtest.h>
#include <algorithm>
#include <stdexcept>
#include <sstream>
#include <iomanip>


/**
 * Helper: iconv_helper
 *
 * Uses the specified iconv object to convert from the specified input buffer.
 * Returns a vector<char> containing the converted bytes.
 *
 * Parameters:
 *  cd      iconv conversion context, result of iconv_open
 *  inbuf   Input buffer; must be allocated with at least count bytes.
 *  count   Number of bytes to convert; must be greater than 0.
 */
std::vector<char>
    iconv_helper(iconv_t cd, char* inbuf, size_t count)
{
    std::vector<char> out(2 * count);

    char* outbuf = &out[0];
    size_t outleft = out.size();
    size_t inleft = count;

    while ((size_t)-1 == iconv(cd, &inbuf, &inleft, &outbuf, &outleft))
    {
        // if errno is E2BIG, need to reallocate buffer
        if (errno == E2BIG)
        {
            size_t oldsize = out.size();
            out.resize(2 * out.size());
            // Have to reposition outbuf
            outbuf = &out[oldsize - outleft];
            outleft += oldsize;
        }
        else if (errno == EINVAL)
        {
            std::stringstream ss;
            ss << "iconv encountered an incomplete character at end of input: ";
            while (inleft--)
                ss << std::hex << "0x" << (int)*(unsigned char*)inbuf++ << " ";
            throw std::runtime_error(ss.str());
        }
        else if (errno == EILSEQ)
        {
            std::stringstream ss;
            ss << "iconv encounterd an invalid character sequence: ";
            while (inleft--)
                ss << std::hex << "0x" << (int)*(unsigned char*)inbuf++ << " ";
            throw std::runtime_error(ss.str());
        }
        else
        {
            throw std::runtime_error("iconv encountered an unknown error");
        }
    }

    // Resize output vector to hold only the converted bytes
    out.resize(out.size() - outleft);
    return out;
}


/**
 * Converts a big-endian UCS2 string to UTF8
 */
std::string ucs2toutf8(const char16_t* ucs2str, size_t count)
{
    if (count == 0)
        return u8"";
    iconv_t conv = iconv_open("UTF-8", "UCS-2BE");
    std::vector<char> out = iconv_helper(conv, (char*)ucs2str, count*2);
    std::string result(&out[0], out.size());
    iconv_close(conv);
    return result;
}

std::string ucs2toutf8(std::u16string const& ucs2str)
{
    return ucs2toutf8(ucs2str.data(), ucs2str.length());
}


/**
 * Converts a UTF-8 string to big-endian UCS-2
 */
std::u16string utf8toucs2(const char* utf8str, size_t count)
{
    if (count == 0)
        return u"";
    iconv_t conv = iconv_open("UCS-2BE", "UTF-8");
    std::vector<char> out = iconv_helper(conv, (char*)utf8str, count);
    // This cast is ugly, but doesn't break strict aliasing since we're casting
    // from char*
    std::u16string result((char16_t*)&out[0], out.size() / 2);
    iconv_close(conv);
    return result;
}

std::u16string utf8toucs2(std::string const& utf8str)
{
    return utf8toucs2(utf8str.data(), utf8str.length());
}


/**
 * Streaming support for std::u16string
 */
std::ostream& operator<<(std::ostream& os, std::u16string const& str)
{
    os << ucs2toutf8(str);
    return os;
}


/*
 * UNIT TESTS
 * 
 */

//
// Because we work with UCS-2 big-endian strings, we may need to transform
// byte order before using them in tests.
//
std::u16string big(std::u16string const& str)
{
    std::u16string copy(str);
    std::transform(copy.begin(), copy.end(), copy.begin(), &ntoh<char16_t>);
    return copy;
}

TEST(UniconvTests, UCS2toUTF8)
{
    std::u16string test = big(u"Some magic runes: ᛥᛯᛘ⌦");
    std::string expected = u8"Some magic runes: ᛥᛯᛘ⌦";

    ASSERT_EQ(expected, ucs2toutf8(test));
}

TEST(UniconvTests, UTF8toUCS2)
{
    std::string test = u8"Some magic runes: ᛥᛯᛘ⌦";
    std::u16string expected = big(u"Some magic runes: ᛥᛯᛘ⌦");

    ASSERT_EQ(expected, utf8toucs2(test));
}

/*
 * Verify that re-conversion works (at least for strings that are both valid
 * UTF-8 and UCS-2).
 */
TEST(UniconvTests, UTF8UCS2Composable)
{
    std::string expected8 = u8"This: \u21BA is a UTF-8 string.";
    EXPECT_EQ(expected8, ucs2toutf8(utf8toucs2(expected8)));

    std::u16string expected16 = u"This: \u21BA is a UCS-2 string.";
    std::u16string actual = utf8toucs2(ucs2toutf8(expected16));

    EXPECT_EQ(expected16, actual);
}

