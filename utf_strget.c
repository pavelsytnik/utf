#include "utf.h"

static int u8_seqlen(char c)
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

// Also unsafe code
char32_t u8_strget(const char8_t *s, size_t i)
{
    char32_t ch = 0;

    while (i-- > 0)
        s += u8_seqlen(*s);

    int len = u8_seqlen(*s);
    for (int i = 0; i < len; i++)
        ch = ch << 8 | *(s + i) & 0xFF;

    return u8tou32(ch);
}

// TODO: append check for null-terminated character
char8_t *u8_strat(const char8_t *s, size_t i)
{
    while (i-- > 0)
        s += u8_seqlen(*s);

    return s;
}
