// uniconv.h
#pragma once

#include <string>
#include <iosfwd>

/**
 * Function: utf8toucs2
 *
 * Converts a UTF-8 encoded string of char to a big-endian UCS-2 encoded string
 * of char16_t.
 */
std::u16string utf8toucs2(std::string const&);
std::u16string utf8toucs2(const char*, size_t);

/**
 * Function: ucs2toutf8
 *
 * Converts a big-endian UCS-2 encoded string of char16_t to a UTF-8 string.
 */
std::string ucs2toutf8(std::u16string const&);
std::string ucs2toutf8(const char16_t*, size_t);

/**
 * Operator << for UTF-16 strings
 *
 * Remove when C++ supports native streaming of std::u16string. (Yeah, right)
 */
std::ostream& operator<<(std::ostream&, std::u16string const&);

