#include "utf.h"

static int utf_u8seqlen(char c)
{
    if ((c & 0x80) == 0x00)
        return 1;
    else if ((c & 0xE0) == 0xC0)
        return 2;
    else if ((c & 0xF0) == 0xE0)
        return 3;
    else if ((c & 0xF8) == 0xF0)
        return 4;
    else
        return 0;
}

// Maybe temporary, maybe not
static uint32_t utf_char8to32(uint32_t u8c)
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
char32_t utf_str8get(const char8_t *s, size_t i)
{
    char32_t ch = 0;

    while (i-- > 0)
        s += utf_u8seqlen(*s);

    int len = utf_u8seqlen(*s);
    for (int i = 0; i < len; i++)
        ch = ch << 8 | *(s + i) & 0xFF;

    return utf_char8to32(ch);
}

// TODO: append check for null-terminated character
char8_t *utf_str8at(const char8_t *s, size_t i)
{
    while (i-- > 0)
        s += utf_u8seqlen(*s);

    return s;
}
