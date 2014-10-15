#if !defined(STDTEXT_CODECS_UTF16LE_CODEC_HPP) // {
#define STDTEXT_CODECS_UTF16LE_CODEC_HPP


#include <stdtext/concepts.hpp>
#include <stdtext/exceptions.hpp>
#include <stdtext/character.hpp>
#include <stdtext/codecs/trivial_codec.hpp>
#include <cstdint>


namespace std {
namespace experimental {
namespace text {


template<Character CT, Code_unit CUT>
struct utf16le_codec {
    using state_type = trivial_codec_state;
    using character_type = CT;
    using code_unit_type = CUT;
    static constexpr int min_code_units = 2;
    static constexpr int max_code_units = 4;

    template<Code_unit_iterator CUIT>
    requires origin::Output_iterator<CUIT, code_unit_type>()
    static void encode(
        state_type &state,
        CUIT &out,
        character_type c,
        int &encoded_code_units)
    {
        encoded_code_units = 0;

        using code_point_type =
            code_point_type_of<character_set_type_of<character_type>>;
        code_point_type cp{c.get_code_point()};

        if (cp >= 0xD800 && cp <= 0xDFFF) {
            throw text_encode_error("Invalid Unicode code point");
        }

        if (cp < 0xFFFF) {
            code_unit_type octet1 = cp & 0xFF;
            code_unit_type octet2 = (cp >> 8) & 0xFF;

            *out++ = octet1;
            ++encoded_code_units;
            *out++ = octet2;
            ++encoded_code_units;
        } else {
            uint_least16_t cu1 = 0xD800 + (((cp - 0x10000) >> 10) & 0x03FF);
            uint_least16_t cu2 = 0xDC00 + ((cp - 0x10000) & 0x03FF);
            
            code_unit_type octet1 = cu1 & 0xFF;
            code_unit_type octet2 = (cu1 >> 8) & 0xFF;
            code_unit_type octet3 = cu2 & 0xFF;
            code_unit_type octet4 = (cu2 >> 8) & 0xFF;

            *out++ = octet1;
            ++encoded_code_units;
            *out++ = octet2;
            ++encoded_code_units;
            *out++ = octet3;
            ++encoded_code_units;
            *out++ = octet4;
            ++encoded_code_units;
        }
    }

    template<Code_unit_iterator CUIT>
    requires origin::Output_iterator<CUIT, code_unit_type>()
    static void rencode(
        state_type &state,
        CUIT &out,
        character_type c,
        int &encoded_code_units)
    {
        encoded_code_units = 0;

        using code_point_type =
            code_point_type_of<character_set_type_of<character_type>>;
        code_point_type cp{c.get_code_point()};

        if (cp >= 0xD800 && cp <= 0xDFFF) {
            throw text_encode_error("Invalid Unicode code point");
        }

        if (cp < 0xFFFF) {
            code_unit_type roctet2 = cp & 0xFF;
            code_unit_type roctet1 = (cp >> 8) & 0xFF;

            *out++ = roctet1;
            ++encoded_code_units;
            *out++ = roctet2;
            ++encoded_code_units;
        } else {
            uint_least16_t cu1 = 0xD800 + (((cp - 0x10000) >> 10) & 0x03FF);
            uint_least16_t cu2 = 0xDC00 + ((cp - 0x10000) & 0x03FF);
            
            code_unit_type roctet4 = cu1 & 0xFF;
            code_unit_type roctet3 = (cu1 >> 8) & 0xFF;
            code_unit_type roctet2 = cu2 & 0xFF;
            code_unit_type roctet1 = (cu2 >> 8) & 0xFF;

            *out++ = roctet1;
            ++encoded_code_units;
            *out++ = roctet2;
            ++encoded_code_units;
            *out++ = roctet3;
            ++encoded_code_units;
            *out++ = roctet4;
            ++encoded_code_units;
        }
    }

    template<Code_unit_iterator CUIT, typename CUST>
    requires origin::Input_iterator<CUIT>()
          && origin::Convertible<origin::Value_type<CUIT>, code_unit_type>()
          && origin::Sentinel<CUST, CUIT>()
    static void decode(
        state_type &state,
        CUIT &in_next,
        CUST in_end,
        character_type &c,
        int &decoded_code_units)
    {
        decoded_code_units = 0;

        using code_point_type =
            code_point_type_of<character_set_type_of<character_type>>;
        code_point_type cp;

        if (in_next == in_end)
            throw text_decode_underflow_error("text decode underflow error");
        code_unit_type octet1 = *in_next++;
        ++decoded_code_units;
        if (in_next == in_end)
            throw text_decode_underflow_error("text decode underflow error");
        code_unit_type octet2 = *in_next++;
        ++decoded_code_units;
        uint_least16_t cu1 = ((octet2 & 0xFF) << 8) | (octet1 & 0xFF);
        if (cu1 >= 0xD800 && cu1 <= 0xDBFF) {
            if (in_next == in_end)
                throw text_decode_underflow_error("text decode underflow error");
            code_unit_type octet3 = *in_next++;
            ++decoded_code_units;
            if (in_next == in_end)
                throw text_decode_underflow_error("text decode underflow error");
            code_unit_type octet4 = *in_next++;
            ++decoded_code_units;
            uint_least16_t cu2 = ((octet4 & 0xFF) << 8) | (octet3 & 0xFF);
            if (cu2 < 0xDC00 || cu2 > 0xDFFF) {
                throw text_decode_error("Invalid UTF-16 code unit sequence");
            }
            cp = 0x10000 + (((cu1 & 0x3FF) << 10) | (cu2 & 0x3FF));
            c.set_code_point(cp);
        } else if (cu1 >= 0xDC00 && cu1 <= 0xDFFF) {
            throw text_decode_error("Invalid UTF-16 code unit sequence");
        } else {
            cp = cu1;
            c.set_code_point(cp);
        }
    }

    template<Code_unit_iterator CUIT, typename CUST>
    requires origin::Input_iterator<CUIT>()
          && origin::Convertible<origin::Value_type<CUIT>, code_unit_type>()
          && origin::Sentinel<CUST, CUIT>()
    static void rdecode(
        state_type &state,
        CUIT &in_next,
        CUST in_end,
        character_type &c,
        int &decoded_code_units)
    {
        decoded_code_units = 0;

        using code_point_type =
            code_point_type_of<character_set_type_of<character_type>>;
        code_point_type cp;

        if (in_next == in_end)
            throw text_decode_underflow_error("text decode underflow error");
        code_unit_type roctet1 = *in_next++;
        ++decoded_code_units;
        if (in_next == in_end)
            throw text_decode_underflow_error("text decode underflow error");
        code_unit_type roctet2 = *in_next++;
        ++decoded_code_units;
        uint_least16_t rcu1 = ((roctet1 & 0xFF) << 8) | (roctet2 & 0xFF);
        if (rcu1 >= 0xDC00 && rcu1 <= 0xDFFF) {
            if (in_next == in_end)
                throw text_decode_underflow_error("text decode underflow error");
            code_unit_type roctet3 = *in_next++;
            ++decoded_code_units;
            if (in_next == in_end)
                throw text_decode_underflow_error("text decode underflow error");
            code_unit_type roctet4 = *in_next++;
            ++decoded_code_units;
            uint_least16_t rcu2 = ((roctet3 & 0xFF) << 8) | (roctet4 & 0xFF);
            if (rcu2 < 0xD800 || rcu2 > 0xDBFF) {
                throw text_decode_error("Invalid UTF-16 code unit sequence");
            }
            cp = 0x10000 + (((rcu2 & 0x3FF) << 10) | (rcu1 & 0x3FF));
            c.set_code_point(cp);
        } else if (rcu1 >= 0xD800 && rcu1 <= 0xDBFF) {
            throw text_decode_error("Invalid UTF-16 code unit sequence");
        } else {
            cp = rcu1;
            c.set_code_point(cp);
        }
    }
};


} // namespace text
} // namespace experimental
} // namespace std


#endif // } STDTEXT_CODECS_UTF16LE_CODEC_HPP
