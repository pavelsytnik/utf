#include "utf.h"

void str_u16tou32(char32_t *restrict dst, const char16_t *restrict src)
{
    while (*src != 0) {
        char32_t ch = 0;
        if (*src >= 0xD800 && *src <= 0xDBFF) {
            ch |= *src++ - 0xD800 << 10;
            ch |= *src++ - 0xDC00;
            ch += 0x10000;
        } else if (*src < 0xDC00 || *src > 0xDFFF) {
            ch |= *src++;
        } else {
            break;
        }
        *dst++ = ch;
    }
    *dst = 0;
}
