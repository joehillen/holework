// requestfields.cpp
//
#include "requestfields.h"
#include "uniconv.h"
#include <gtest/gtest.h>

namespace xim { namespace network {

/*
 * Specialized number readers
 */

size_t readBool(boost::asio::streambuf& buf, bool& out) {
    int8_t temp;
    size_t result = readNumber<int8_t>(buf, temp);
    out = (temp != 0);
    return result;
}
size_t readByte(boost::asio::streambuf& buf, int8_t& out) {
    return readNumber<int8_t>(buf, out);
}
size_t readShort(boost::asio::streambuf& buf, int16_t& out) {
    return readNumber<int16_t>(buf, out);
}
size_t readInt(boost::asio::streambuf& buf, int32_t& out) {
    return readNumber<int32_t>(buf, out);
}
size_t readLong(boost::asio::streambuf& buf, int64_t& out) {
    return readNumber<int64_t>(buf, out);
}
size_t readFloat(boost::asio::streambuf& buf, float& out) {
    return readNumber<float>(buf, out);
}
size_t readDouble(boost::asio::streambuf& buf, double& out) {
    return readNumber<double>(buf, out);
}

/*
 * Specialized string readers
 */

std::string NullConversion(std::string const& str)
{
    return str;
}

size_t readStringUtf8(boost::asio::streambuf& buf, std::string& out)
{
    return readString<char, NullConversion>(buf, out);
}

size_t readStringUcs2(boost::asio::streambuf& buf, std::string& out)
{
    return readString<char16_t, ucs2toutf8>(buf, out);
}

}} // namespace xim::network


/*
 * UNIT TESTS
 */

using namespace xim::network;

TEST(RequestFieldTests, ByteField)
{
    boost::asio::streambuf b;
    std::ostream os(&b);
    os.put(0x42);

    int8_t val = 0;

    ASSERT_EQ(0, readByte(b, val));
    ASSERT_EQ(0x42, val);
}

TEST(RequestFieldTests, ShortField)
{
    boost::asio::streambuf b;
    std::ostream os(&b);
    os.put(0x10);
    os.put(0x80);

    int16_t val = 0;

    ASSERT_EQ(0, readShort(b, val));
    ASSERT_EQ(0x1080, val);
}

TEST(RequestFieldTests, IntField)
{
    boost::asio::streambuf b;
    std::ostream os(&b);
    os.put(0x10);
    os.put(0x20);
    os.put(0x30);
    os.put(0x40);

    int32_t val = 0;

    ASSERT_EQ(0, readInt(b, val));
    ASSERT_EQ(0x10203040, val);
}

TEST(RequestFieldTests, LongField)
{
    boost::asio::streambuf b;
    std::ostream os(&b);
    os.put(0x10);
    os.put(0x20);
    os.put(0x30);
    os.put(0x40);
    os.put(0x50);
    os.put(0x60);
    os.put(0x70);
    os.put(0x80);

    int64_t val = 0;

    ASSERT_EQ(0, readLong(b, val));
    ASSERT_EQ(0x1020304050607080LLU, val);
}

TEST(RequestFieldTests, DoubleField)
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

    ASSERT_EQ(0, readDouble(b, val));
    // This number should be exactly representable
    ASSERT_EQ(1337.1337, val);
}

TEST(RequestFieldTests, DoubleFieldContinuable)
{
    boost::asio::streambuf b;
    std::ostream os(&b);

    double val = 0.0d;

    // All 8 bytes should still be required at first
    ASSERT_EQ(8, readDouble(b, val));
    ASSERT_EQ(0.0, val);

    // Double value 4.6855785559756050e-277
    // Put only the first three bytes in the buffer
    os.put(0x06);
    os.put(0x90);
    os.put(0x9c);

    // Should need five more bytes
    ASSERT_EQ(5, readDouble(b, val));
    ASSERT_EQ(0.0, val);

    os.put(0xa5);

    ASSERT_EQ(4, readDouble(b, val));
    ASSERT_EQ(0.0, val);

    os.put(0x45);
    os.put(0x8c);
    os.put(0xa9);
    os.put(0xc6);

    ASSERT_EQ(0, readDouble(b, val));
    // This value should be exactly represented so direct equality is OK
    ASSERT_EQ(4.6855785559756050e-277, val);
}

TEST(RequestFieldTests, UTF8StringField)
{
    boost::asio::streambuf b;
    std::ostream os(&b);

    std::string test("I'm a potato!");

    os.put(test.length()>>8);
    os.put(test.length()&0xff);
    os << test;

    std::string val;

    ASSERT_EQ(0, readStringUtf8(b, val));
    ASSERT_EQ(test, val);
}

TEST(RequestFieldTests, UCS2StringField)
{
    boost::asio::streambuf b;
    std::ostream os(&b);

    std::u16string test(u"Unicode is better than pancakes.");
    std::string expected(u8"Unicode is better than pancakes.");

    // Transform characters of string to network byte order
    std::transform(test.begin(), test.end(), test.begin(), &ntoh<char16_t>);

    os.put(test.length()>>8);
    os.put(test.length()&0xff);
    os.write((char*)test.data(), test.length()*2);

    std::string val;
    ASSERT_EQ(0, readStringUcs2(b, val));
    ASSERT_EQ(expected, val);
}

/*
 * Verify that UTF-8 strings are read correctly through buffer underruns
 */
TEST(RequestFieldTests, UtfStringContinuable)
{
    boost::asio::streambuf b;
    std::ostream os(&b);

    /*
     * For this test, one quatrain of the Rubaiyat of Omar Khayyam, in Farsi
     * and Fitzgerald's English, plus one quatrain of the Rubaiyat of a Persian
     * kitten, in plain and simple everyday unoriental speech.
     */
    std::string teststr(
        u8"Why, all the Saints and Sages who discuss'd\n"
        "Of the Two Worlds so wisely -- they are thrust\n"
        "Like foolish Prophets forth; their Words to Scorn\n"
        "Are scatter'd, and their Mouths are stopt with Dust.\n"
        "\n"
        "آنانكه ز پيش رفته‌اند اى ساقى\n"
        "درخاك غرور خفته‌اند اى ساقى\n"
        "رو باده خور و حقيقت از من بشنو\n"
        "باد است هرآنچه گفته‌اند اى ساقى\n"
        "\n"
        "They say the Lion and the Lizard keep\n"
        "The Courts where Jamshyd gloried and drank deep.\n"
        "The Lion is my cousin; I don't know\n"
        "Who Jamshyd is--nor shall it break my sleep.\n");

    os.put(teststr.length() >> 8);
    os.put(teststr.length() & 0xFF);

    std::string val;

    // Only write the first 42 bytes at first; the first attempt to read should
    // then come up len-42 bytes short and consume all 42 available bytes.
    os << teststr.substr(0, 42);
    size_t bytesleft = readStringUtf8(b, val);

    // After each attempt to read from an incomplete buffer, assert:
    //  - the number of bytes needed is the tag length less the buffer size
    //  - nothing has been consumed from the buffer
    //  - the output string has not been changed
    ASSERT_EQ(teststr.length() - 42, bytesleft);
    ASSERT_EQ(44, b.size());
    ASSERT_TRUE(val.empty());

    // Write the next 112 bytes and try reading from the buffer again
    os << teststr.substr(42, 112);
    bytesleft = readStringUtf8(b, val);
    ASSERT_EQ(teststr.length() - 42 - 112, bytesleft);
    ASSERT_EQ(44+112, b.size());
    ASSERT_TRUE(val.empty());

    // Finally, write the rest of the string
    os << teststr.substr(42+112);
    bytesleft = readStringUtf8(b, val);
    ASSERT_EQ(0, bytesleft);
    ASSERT_EQ(0, b.size());

    // Check that the read string is the same as the original
    ASSERT_EQ(teststr, val);
}

/*
 * Verify that UCS-2 strings are read correctly across multiple buffers
 */

// These macros are kinda evil, but their purpose is to allow us to use the
// same string literal with both u and u8 prefixes without needing to repeat
// it.

#define XUTF8(s) (u8 ## s)
#define XUTF16(s) (u ## s)
#define UTF8(s) XUTF8(s)
#define UTF16(s) XUTF16(s)

#define BEOWULF \
    "Hwæt! We Gardena in geardagum,\n" \
    "þeodcyninga, þrym gefrunon,\n" \
    "hu ða æþelingas ellen fremedon.\n" \
    "Oft Scyld Scefing sceaþena þreatum,\n" \
    "\n" \
    "monegum mægþum, meodosetla ofteah,\n" \
    "egsode eorlas. Syððan ærest wearð\n" \
    "feasceaft funden,he þæs frofre gebad,\n" \
    "weox under wolcnum, weorðmyndum þah,\n" \
    "oðþæt him æghwylc þara ymbsittendra\n" \
    "\n" \
    "ofer hronrade hyran scolde,\n" \
    "gomban gyldan. þæt wæs god cyning!\n" \
    "Ðæm eafera wæs æfter cenned,\n" \
    "geong in geardum, þone god sende\n" \
    "folce to frofre; fyrenðearfe ongeat\n" \
    "\n" \
    "þe hie ær drugon aldorlease\n" \
    "lange hwile. Him þæs liffrea,\n" \
    "wuldres wealdend, woroldare forgeaf;\n" \
    "Beowulf wæs breme (blæd wide sprang),\n" \
    "Scyldes eafera Scedelandum in.\n"

#include <iostream>

TEST(RequestFieldTests, UCS2FieldContinuable)
{
    boost::asio::streambuf b;
    std::ostream os(&b);

    std::u16string test = UTF16(BEOWULF);
    std::string expected = UTF8(BEOWULF);

    // Transform the UTF16/UCS2 version to network byte order
    std::transform(test.begin(), test.end(), test.begin(), &ntoh<char16_t>);

    os.put(test.length() >> 8);
    os.put(test.length() & 0xFF);

    std::string val;

    // Write 31 bytes to the buffer and attempt reading. The field should
    // capture only 30 bytes, since the remainder is not a complete char16_t.

    // Write 31 bytes to the buffer and attempt reading
    char* data = (char*)test.data();
    os.write(data, 31);
    size_t bytesleft = readStringUcs2(b, val);

    ASSERT_EQ(2 * test.length() - 31, bytesleft);
    ASSERT_EQ(33, b.size());
    ASSERT_TRUE(val.empty());

    // Write the next 101 bytes and try reading from the buffer again
    os.write(data + 31, 101);
    bytesleft = readStringUcs2(b, val);

    ASSERT_EQ(2 * test.length() - 31 - 101, bytesleft);
    ASSERT_EQ(33 + 101, b.size());
    ASSERT_TRUE(val.empty());

    // Finally, write the rest of the string
    os.write(data + 31 + 101, test.length()*2 - 31 - 101);
    bytesleft = readStringUcs2(b, val);
    ASSERT_EQ(0, bytesleft);
    ASSERT_EQ(0, b.size());

    // Check that the read string equals the UTF-8 version of the original
    ASSERT_EQ(expected, val);
}

/*
 * Verify that multiple fields can be read successively from a single buffer
 */
TEST(RequestFieldTests, MultipleFields)
{
    boost::asio::streambuf b;
    std::ostream os(&b);

    // Fields:
    //  (string8) "Avogadro's number (not Avocado) is:"
    //  (float)   0x66FF0C30 = 6.0221414e+23
    //  (byte)    0x2A
    //  (byte)    0xFE
    //  (long)    0xDEADBEEF00C0FFEE
    std::string avocado("Avogadro's number (not Avocado) is:");
    os.put(avocado.length() >> 8);
    os.put(avocado.length() & 0xFF);
    os << avocado;
    std::vector<uint8_t> bytes = { 0x66, 0xFF, 0x0C, 0x30,
        0x2A, 0xFE, 0xDE, 0xAD, 0xBE, 0xEF, 0x00, 0xC0, 0xFF, 0xEE };
    os.write((char*)&bytes[0], bytes.size());

    size_t bufsize = b.size();

    std::string msg;
    float avogadro = 0.0f;
    int8_t foo = 0;
    int8_t bar = 0;
    int64_t baz = 0;

    ASSERT_EQ(0, readStringUtf8(b, msg));
    ASSERT_EQ(avocado, msg);
    ASSERT_EQ(bufsize - 2 - avocado.length(), b.size());
    bufsize -= 2 + avocado.length();

    ASSERT_EQ(0, readFloat(b, avogadro));
    ASSERT_FLOAT_EQ(6.0221414e+23, avogadro);
    ASSERT_EQ(bufsize - 4, b.size());
    bufsize -= 4;

    ASSERT_EQ(0, readByte(b, foo));
    ASSERT_EQ(0x2A, foo);
    ASSERT_EQ(bufsize - 1, b.size());
    bufsize -= 1;

    ASSERT_EQ(0, readByte(b, bar));
    ASSERT_EQ((int8_t)0xFE, bar);
    ASSERT_EQ(bufsize - 1, b.size());
    bufsize -= 1;

    ASSERT_EQ(0, readLong(b, baz));
    ASSERT_EQ(0xDEADBEEF00C0FFEE, baz);
    ASSERT_EQ(0, b.size());
}

