#include "utf.h"

void str_u8tou32(char32_t *restrict dst, const char8_t *restrict src)
{
    while (*src != 0) {
        char32_t ch = 0;
        if ((*src & 0x80) == 0x00) {
            ch |= *src++;
        } else if ((*src & 0xE0) == 0xC0) {
            ch |= (*src++ & 0x1F) << 6;
            ch |=  *src++ & 0x3F;
        } else if ((*src & 0xF0) == 0xE0) {
            ch |= (*src++ & 0x0F) << 12;
            ch |= (*src++ & 0x3F) << 6;
            ch |=  *src++ & 0x3F;
        } else if ((*src & 0xF8) == 0xF0) {
            ch |= (*src++ & 0x07) << 18;
            ch |= (*src++ & 0x3F) << 12;
            ch |= (*src++ & 0x3F) << 6;
            ch |=  *src++ & 0x3F;
        } else {
            break;
        }
        *dst++ = ch;
    }
    *dst = 0;
}
