// request.cpp
//
#include "request.h"
#include "requestfields.h"
#include "../events.h"

#include <string>

namespace boostcraft { namespace network {

/*
 * Excerpt from the Official Unwrittten Boostcraft Style Guide:
 *
 * Preprocessor Macros
 * -------------------
 * These are to be eschewed whenever there is a reasonable alternative. Macros
 * have many pitfalls, including obfuscation of source code and loss of type
 * safety. If there's a way to accomplish your goal without using macros, don't
 * use macros.
 *
 * Here are some great things to NOT use macros for:
 *
 *  Preprocessor Macro Use                              Punishment
 *  ---------------------------------------------------------------------------
 *   #defining constants                                Slap on the wrist
 *   "Inlining" function-like code                      That's a paddling
 *   Hiding ugly code                                   Twenty lashes
 *   Generic programming                                Waterboarding
 *   Implementing mini-DSLs in the preprocessor         Firing squad
 *
 * That said, sometimes using macros can really improve the readability and
 * maintainability of a piece of code. When that happens, and there's not any
 * obvious way of refactoring the code to obviate the need for macros, just use
 * some macros rather than write ugly code.
 *
 * So, with that in mind...
 */

/**
 * Ugly code-hiding mini-DSL for packet field specs!
 *
 * These macros ease the definition of data layouts for request packets. They
 * can only be used in the body of Request subclasses.
 *
 *  BEGIN_FIELDS    Marks the start of fields section in a Request class.
 *  END_FIELDS      REQUIRED to end the fields section.
 *  BYTE,SHORT,ETC. Binds a member variable of the class to a typed position
 *                  in the packet's data layout.
 *
 *  Example:
 *
 *  struct SomePacket : public Request {
 *      int32_t theNumber;
 *      std::string theString;
 *      BEGIN_FIELDS
 *          INT         (theNumber)
 *          STRING_UTF8 (theString)
 *      END_FIELDS
 *  };
 */
#define BEGIN_FIELDS    size_t read(boost::asio::streambuf & buf) \
                        { \
                            int cur_field = 0; \

#define FIELD(type,v)       if (this->fields_read_ == cur_field++) { \
                                size_t needed = type(buf, v); \
                                if (needed) \
                                    return needed; \
                                else \
                                    ++this->fields_read_; \
                            }

#define END_FIELDS          return 0; \
                        }

#define BYTE(v)     FIELD(readByte, v)
#define SHORT(v)    FIELD(readShort, v)
#define INT(v)      FIELD(readInt, v)
#define LONG(v)     FIELD(readLong, v)
#define FLOAT(v)    FIELD(readFloat, v)
#define DOUBLE(v)   FIELD(readDouble, v)
#define STRING_UTF8(v) FIELD(readStringUtf8, v)
#define STRING_UCS2(v) FIELD(readStringUcs2, v)


struct LoginRequest : public Request
{
    int32_t version;
    std::string username;
    int64_t seed;
    int8_t dimension;

    BEGIN_FIELDS
        INT         (version)
        STRING_UCS2 (username)
        LONG        (seed)
        BYTE        (dimension)
    END_FIELDS

    void dispatch()
    {
        //LoginEvent e(version, username, seed, dimension);
        //async_fire(e);
    }
};


}} // namespace boostcraft::network
