#include "utf.h"

void str_u32tou16(char16_t *restrict dst, const char32_t *restrict src)
{
    while (*src != 0) {
        if (*src < 0x10000) {
            *dst++ = *src;
        } else if (*src < 0x110000) {
            char32_t ch = *src - 0x10000;
            *dst++ = (ch >> 10) + 0xD800;
            *dst++ = (ch & 0x3FF) + 0xDC00;
        } else {
            break;
        }
        src++;
    }
    *dst = 0;
}
