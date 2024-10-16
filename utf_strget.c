#include "utf_strget.h"

// Maybe temporary, maybe not
static uint32_t utf_ch_8_to_32_(uint32_t u8c)
{
    if ((u8c & 0x80) == 0x00) {
        return u8c;
    } else if ((u8c >> 8 & 0xE0) == 0xC0) {
        uint32_t out = 0;
        out |= (u8c >> 8 & 0x1F) << 6;
        out |= u8c & 0x3F;
        return out;
    } else if ((u8c >> 16 & 0xF0) == 0xE0) {
        uint32_t out = 0;
        out |= (u8c >> 16 & 0x0F) << 12;
        out |= (u8c >> 8  & 0x3F) << 6;
        out |= u8c & 0x3F;
        return out;
    } else if ((u8c >> 24 & 0xF8) == 0xF0) {
        uint32_t out = 0;
        out |= (u8c >> 24 & 0x07) << 18;
        out |= (u8c >> 16 & 0x3F) << 12;
        out |= (u8c >> 8  & 0x3F) << 6;
        out |= u8c & 0x3F;
        return out;
    } else {
        return 0xFFFFFFFF;
    }
}

// Also unsafe code
utf_c32 utf_8_strget(const utf_c8 *s, size_t i)
{
    utf_c32 ch = 0;

    while (i-- > 0)
        s += utf_8_length_from_lead(*s);

    int len = utf_8_length_from_lead(*s);
    for (int i = 0; i < len; i++)
        ch = ch << 8 | *(s + i) & 0xFF;

    return utf_ch_8_to_32_(ch);
}

// TODO: append check for null-terminated character
utf_c8 *utf_8_strat(const utf_c8 *s, size_t i)
{
    while (i-- > 0)
        s += utf_8_length_from_lead(*s);

    return s;
}
