#include "utf.h"

void str_u32tou8(char8_t *restrict dst, const char32_t *restrict src)
{
    while (*src != 0) {
        if (*src < 0x80) {
            *dst++ = *src;
        } else if (*src < 0x800) {
            *dst++ = *src >> 6 | 0xC0;
            *dst++ = *src & 0x3F | 0x80;
        } else if (*src < 0x10000) {
            *dst++ = *src >> 12 | 0xE0;
            *dst++ = *src >> 6 & 0x3F | 0x80;
            *dst++ = *src & 0x3F | 0x80;
        } else if (*src < 0x110000) {
            *dst++ = *src >> 18 | 0xF0;
            *dst++ = *src >> 12 & 0x3F | 0x80;
            *dst++ = *src >> 6 & 0x3F | 0x80;
            *dst++ = *src & 0x3F | 0x80;
        } else {
            break;
        }
        src++;
    }
    *dst = 0;
}
