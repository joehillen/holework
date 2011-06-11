// pdl.cpp
//
//
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/home/phoenix/object/static_cast.hpp>
#include <memory>

#include "event/types.h"
#include "log.h"
#include "network/response.h"
#include "player.h"
#include <sstream>

using namespace boostcraft::event;

namespace boostcraft {
namespace pdl {

    void chat_handler(ChatEvent&);

    void init()
    {
        listen(chat_handler);
        log(INFO, "PDL", "Packet Description Language module initialized");
    }

    template<typename Iterator>
    network::Response parse_response(Iterator first, Iterator last)
    {
        using namespace boost::spirit::qi;
        using boost::spirit::_1;
        using boost::spirit::ascii::space;
        using boost::phoenix::ref;
        using boost::phoenix::static_cast_;

        ///////////////////////////////////////
        // Response object to fill in
        network::Response r;

        typedef rule<Iterator, ascii::space_type> prod;

        ///////////////////////////////////////
        // Create parser
        prod BYTE       = 'b' >> int_       [ ref(r) << static_cast_<int8_t>(_1) ];
        prod SHORT      = 's' >> short_     [ ref(r) << static_cast_<int16_t>(_1) ];
        prod INTEGER    = 'i' >> int_       [ ref(r) << static_cast_<int32_t>(_1) ];
        prod LONG       = 'l' >> long_      [ ref(r) << static_cast_<int64_t>(_1) ];
        prod FLOAT      = 'f' >> float_     [ ref(r) << _1 ];
        prod DOUBLE     = 'd' >> double_    [ ref(r) << _1 ];

        prod PRIMITIVE  = BYTE | SHORT | INTEGER | LONG | FLOAT | DOUBLE;
        prod TAG        = '@' >> int_       [ ref(r) << static_cast_<int8_t>(_1) ];
        prod PACKET     = TAG >> *PRIMITIVE;

        bool success = phrase_parse(
                first, last,    // range iterators
                PACKET,         // root symbol of parser
                space);         // whitespace parser

        if (!success || first != last)
            throw std::runtime_error("Message parse failed.");

        return r;
    }

    void chat_handler(ChatEvent& e)
    {
        if (e.message.at(0) != '@')
            return;
        try
        {
            e.player->deliver(parse_response(e.message.begin(), e.message.end()));
        }
        catch(std::exception& ex)
        {
            e.player->deliver(network::chatmessage(
                    "[§4ERROR§f] §5" + std::string(ex.what())));
        }
        e.canceled = true;
    }

}} // namespace boostcraft::pdl
