// uniconv.cpp

#include "uniconv.h"

// TODO: windows users will need to provide an alternative implementation!
#include <iconv.h>

std::string ucs2toutf8(std::string const& ucs2str)
{
    // TODO: magic
    iconv_t conv = iconv_open("UTF-8", "UCS-2BE");

    size_t inleft = ucs2str.length();

    // Conveniently, no UCS2 string can produce a UTF8 sequence
    // larger than twice its size
    size_t outbufsize = inleft*2;

    char* inbuf = const_cast<char*>(ucs2str.c_str());

    char* outbuf = new char[outbufsize];
    char* original_outbuf = outbuf;
    size_t outleft = outbufsize;

    iconv(conv, &inbuf, &inleft, &outbuf, &outleft);

    size_t outlen = outbufsize - outleft; 

    iconv_close(conv);

    std::string output = std::string(outbuf, outlen);

    delete[] original_outbuf;

    return output;
}

std::string utf8toucs2(std::string const& utf8str)
{
    // TODO: magic
    iconv_t conv = iconv_open("UCS-2BE", "UTF-8");

    size_t inleft = utf8str.length();

    size_t outbufsize = inleft*2;

    char* inbuf = const_cast<char*>(utf8str.c_str());

    char* outbuf = new char[outbufsize];
    char* original_outbuf = outbuf;
    size_t outleft = outbufsize;

    iconv(conv, &inbuf, &inleft, &outbuf, &outleft);

    size_t outlen = outbufsize - outleft; 

    iconv_close(conv);

    std::string output = std::string(outbuf, outlen);

    delete[] original_outbuf;

    return output;
}
