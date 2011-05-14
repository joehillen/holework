// uniconv.cpp

#include "uniconv.h"

// TODO: windows users will need to provide an alternative implementation!
#include <iconv.h>
#include <gtest/gtest.h>

std::string ucs2toutf8(std::u16string const& ucs2str)
{
    // TODO: magic
    iconv_t conv = iconv_open("UTF-8", "UCS-2BE");

    size_t inleft = ucs2str.length();

    // Conveniently, no UCS2 string can produce a UTF8 sequence
    // larger than twice its size
    size_t outbufsize = inleft*2;

    char* inbuf = reinterpret_cast<char*>(
                    const_cast<char16_t*>(ucs2str.c_str()));
                    

    char* outbuf = new char[outbufsize];
    char* outptr = outbuf;
    size_t outleft = outbufsize;

    iconv(conv, &inbuf, &inleft, &outptr, &outleft);

    size_t outlen = outbufsize - outleft; 

    iconv_close(conv);

    std::string output = std::string(outbuf, outlen);

    delete[] outbuf;

    return output;
}

std::u16string utf8toucs2(std::string const& utf8str)
{
    // TODO: magic
    iconv_t conv = iconv_open("UCS-2BE", "UTF-8");

    size_t inleft = utf8str.length();

    size_t outbufsize = inleft*2;

    char* inbuf = const_cast<char*>(utf8str.c_str());

    char* outbuf = new char[outbufsize];
    char* outptr = outbuf;
    size_t outleft = outbufsize;

    size_t result = iconv(conv, &inbuf, &inleft, &outptr, &outleft);

    size_t outlen = outbufsize - outleft; 

    iconv_close(conv);

    std::u16string output(reinterpret_cast<char16_t*>(outbuf), outlen);

    delete[] outbuf;

    return output;
}

TEST(TestUtf8ToUcs2, test1)
{
    // The code points we're using are in the first 64K, so UTF-16 and UCS2
    // will be identical (also no surrogate pairs)
    std::string test_str = u8"This is a string: \u2326";

    ASSERT_STREQ(ucs2toutf8(utf8toucs2(test_str)).c_str(), test_str.c_str());
}
