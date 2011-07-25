// request.cpp
//
#include "request.h"
#include "requestfields.h"

#include "player.h"
#include "event/types.h"
#include "network/response.h"

#include <string>

namespace xim {
namespace network {

using namespace xim::event;

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

#define BOOLEAN(v)  FIELD(readBool, v)
#define BYTE(v)     FIELD(readByte, v)
#define SHORT(v)    FIELD(readShort, v)
#define INT(v)      FIELD(readInt, v)
#define LONG(v)     FIELD(readLong, v)
#define FLOAT(v)    FIELD(readFloat, v)
#define DOUBLE(v)   FIELD(readDouble, v)
#define STRING_UTF8(v) FIELD(readStringUtf8, v)
#define STRING_UCS2(v) FIELD(readStringUcs2, v)

namespace packets {

/*
 * 0x00 KEEP ALIVE
 */
struct KeepAlive : public Request
{
    size_t read(boost::asio::streambuf&) { return 0; }
    void dispatch(player_ptr p) const
    {
        // No event dispatch on keepalive; just ping back
        p->deliver(keepalive());
    }
};

/*
 * 0x01 LOGIN REQUEST
 */
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

    void dispatch(player_ptr p) const
    {
        LoginRequestEvent e(p, username, version);
        async_fire(e);
    }
};

/*
 * 0x02 HANDSHAKE
 */
struct Handshake : public Request
{
    std::string username;

    BEGIN_FIELDS
        STRING_UCS2 (username)
    END_FIELDS

    void dispatch(player_ptr p) const
    {
        p->handshake(username);
    }
};

/*
 * 0x03 CHAT MESSAGE
 */
struct ChatMessage : public Request
{
    std::string message;

    BEGIN_FIELDS
        STRING_UCS2 (message)
    END_FIELDS

    void dispatch(player_ptr p) const
    {
        ChatEvent e(p, message);
        async_fire(e); 
    }
};

/*
 * 0x07 USE ENTITY
 */
struct UseEntity : public Request
{
    int32_t user;
    int32_t target;
    bool leftclick;

    BEGIN_FIELDS
        INT         (user)
        INT         (target)
        BOOLEAN     (leftclick)
    END_FIELDS

    void dispatch(player_ptr p) const
    {
        // TODO: implement dispatch
    }
};

/*
 * 0x09 RESPAWN
 */
struct Respawn : public Request
{
    int8_t world;

    BEGIN_FIELDS
        BYTE        (world)
    END_FIELDS

    void dispatch(player_ptr p) const
    {
        // TODO: respawn event
    }
};

/*
 * 0x0A PLAYER ONGROUND
 */
struct PlayerOnGround : public Request
{
    bool onground;

    BEGIN_FIELDS
        BOOLEAN     (onground)
    END_FIELDS

    void dispatch(player_ptr p) const
    {
        PlayerOnGroundEvent e(p, onground);
        async_fire(e);
    }
};

/*
 * 0x0B PLAYER POSITION
 */
struct PlayerPosition : public Request
{
    double x, y, z, stance;
    bool onground;

    BEGIN_FIELDS
        DOUBLE      (x)
        DOUBLE      (y)
        DOUBLE      (stance)
        DOUBLE      (z)
        BOOLEAN     (onground)
    END_FIELDS

    void dispatch(player_ptr p) const
    {
        p->updatePosition({x, z, y});

        PlayerOnGroundEvent g(p, onground);
        async_fire(g);

        PlayerPositionEvent e(p, {x, z, y});
        async_fire(e);
    }
};

/*
 * 0x0C PLAYER LOOK 
 */
struct PlayerLook : public Request
{
    float pitch, yaw;
    bool onground;

    BEGIN_FIELDS
        FLOAT       (yaw)
        FLOAT       (pitch)
        BOOLEAN     (onground)
    END_FIELDS

    void dispatch(player_ptr p) const
    {
        PlayerOnGroundEvent g(p, onground);
        async_fire(g);

        PlayerLookEvent e(p, yaw, pitch);
        async_fire(e);
    }
};

/*
 * 0x0D PLAYER POSITION AND LOOK
 */
struct PlayerPositionAndLook : public Request
{
    double x, y, z, stance;
    float pitch, yaw;
    bool onground;

    BEGIN_FIELDS
        DOUBLE      (x)
        DOUBLE      (y)
        DOUBLE      (stance)
        DOUBLE      (z)
        FLOAT       (yaw)
        FLOAT       (pitch)
        BOOLEAN     (onground)
    END_FIELDS

    void dispatch(player_ptr p) const
    {
        p->updatePosition({x, z, y});

        PlayerOnGroundEvent g(p, onground);
        async_fire(g);

        PlayerLookEvent l(p, yaw, pitch);
        async_fire(l);

        PlayerPositionEvent e(p, {x, z, y});
        async_fire(e);
    }
};

/*
 * 0x0E PLAYER DIGGING
 */
struct Digging : public Request
{
    int8_t status;
    int32_t x;
    int8_t  y;
    int32_t z;
    int8_t face;

    BEGIN_FIELDS
        BYTE        (status)
        INT         (x)
        BYTE        (y)
        INT         (z)
        BYTE        (face)
    END_FIELDS

    void dispatch(player_ptr p) const
    {
        if (status == 0)
            p->deliver(chatmessage("I'm a dwarf and I'm digging a hole!"));
        else if (status == 2)
            p->deliver(chatmessage("Diggy diggy hole!"));
        else if (status == 4)
            p->deliver(chatmessage("Give a hoot; don't pollute!"));
    }
};

/*
 * 0x0F PLAYER BLOCK PLACEMENT
 */
struct BlockPlacement : public Request
{
    int32_t x, z;
    int8_t y;
    int8_t direction;
    int16_t itemid;
    int8_t amount;
    int16_t damage;

    BEGIN_FIELDS
        INT         (x)
        BYTE        (y)
        INT         (z)
        BYTE        (direction)
        SHORT       (itemid)
        if (itemid != -1) {
            BYTE        (amount)
            SHORT       (damage)
        }
    END_FIELDS

    void dispatch(player_ptr p) const
    {

    }
};

/*
 * 0x10 HOLDING CHANGE
 */
struct HoldingChange : public Request
{
    int16_t slot;

    BEGIN_FIELDS
       SHORT        (slot)
    END_FIELDS

    void dispatch(player_ptr p) const
    {
        // TODO: implement dispatch
        Response r;
        r << (int8_t)0x36;
        r << (int32_t)0 << (int16_t)1 << (int32_t)0;
        r << (int8_t)0 << (int8_t)16;
        p->deliver(r);
    }
};

/*
 * 0x12 ENTITY ANIMATION
 */
struct Animation : public Request
{
    int32_t eid;
    int8_t anim;

    BEGIN_FIELDS
        INT         (eid)
        BYTE        (anim)
    END_FIELDS

    void dispatch(player_ptr p) const
    {
        // TODO: implement dispatch
    }
};

/*
 * 0x13 STANCE CHANGE
 */
struct Stance : public Request
{
    int32_t eid;
    int8_t action;  // 1 = crouch, 2 = uncrouch, 3 = leave bed

    BEGIN_FIELDS
        INT         (eid)
        BYTE        (action)
    END_FIELDS

    void dispatch(player_ptr p) const
    {
        // TODO: implement dispatch
    }
};

/*
 * 0x65 CLOSE WINDOW
 */
struct CloseWindow : public Request
{
    int8_t windowid;

    BEGIN_FIELDS
        BYTE        (windowid)
    END_FIELDS

    void dispatch(player_ptr p) const
    {
        // TODO: implement dispatch
    }
};

/*
 * 0x66 WINDOW CLICK
 */
struct ClickWindow : public Request
{
    int8_t windowid;
    int16_t slot;
    bool rclick;
    bool shift;
    int16_t actionid;
    int16_t itemid;
    int8_t  itemcount;
    int16_t itemuses;

    BEGIN_FIELDS
        BYTE        (windowid)
        SHORT       (slot)
        BOOLEAN     (rclick)
        SHORT       (actionid)
        BOOLEAN     (shift)
        SHORT       (itemid)
        if (itemid != -1)
        {
            BYTE    (itemcount)
            SHORT   (itemuses)
        }
    END_FIELDS

    void dispatch(player_ptr p) const
    {
        // TODO: implement dispatch
    }
};

/*
 * 0xFF DISCONNECT
 */
struct Disconnect : public Request
{
    std::string message;

    BEGIN_FIELDS
        STRING_UCS2 (message)
    END_FIELDS

    void dispatch(player_ptr p) const
    {
        p->stop(message);
    }
};


} // namespace packets

#define REQUEST_CASE(num, type) case num: return ptr(new type());

std::unique_ptr<Request> makerequest(int type)
{
    typedef std::unique_ptr<Request> ptr;
    using namespace packets;

    switch(type)
    {
        REQUEST_CASE(0x00, KeepAlive)
        REQUEST_CASE(0x01, LoginRequest)
        REQUEST_CASE(0x02, Handshake)
        REQUEST_CASE(0x03, ChatMessage)
        REQUEST_CASE(0x07, UseEntity)
        REQUEST_CASE(0x09, Respawn)
        REQUEST_CASE(0x0A, PlayerOnGround)
        REQUEST_CASE(0x0B, PlayerPosition)
        REQUEST_CASE(0x0C, PlayerLook)
        REQUEST_CASE(0x0D, PlayerPositionAndLook)
        REQUEST_CASE(0x0E, Digging)
        REQUEST_CASE(0x0F, BlockPlacement)
        REQUEST_CASE(0x10, HoldingChange)
        REQUEST_CASE(0x12, Animation)
        REQUEST_CASE(0x13, Stance)
        REQUEST_CASE(0x65, CloseWindow)
        REQUEST_CASE(0x66, ClickWindow)
        REQUEST_CASE(0xFF, Disconnect)

    default:
        {
            std::stringstream ss;
            ss << "Unrecognized packet ID " << std::hex << type;
            throw std::runtime_error(ss.str());
        }
    }
}

}} // namespace xim::network
