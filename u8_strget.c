#include "utf.h"

#define CHAR_PTR_TO_UTF8 (utf8_t) (unsigned char)

// Also unsafe code
utf32_t u8_strget(const char *s, size_t i)
{
    size_t shift = 0;
    utf8_t ch;

    while (i-- > 0) {
        if ((s[shift] & 0x80) == 0x00) {
            shift += 1;
        } else if ((s[shift] & 0xE0) == 0xC0) {
            shift += 2;
        } else if ((s[shift] & 0xF0) == 0xE0) {
            shift += 3;
        } else if ((s[shift] & 0xF8) == 0xF0) {
            shift += 4;
        }
    }

    if ((s[shift] & 0x80) == 0x00) {
        ch = CHAR_PTR_TO_UTF8 s[shift];
    } else if ((s[shift] & 0xE0) == 0xC0) {
        ch = CHAR_PTR_TO_UTF8 s[shift + 0] << 8 |
             CHAR_PTR_TO_UTF8 s[shift + 1];
    } else if ((s[shift] & 0xF0) == 0xE0) {
        ch = CHAR_PTR_TO_UTF8 s[shift + 0] << 16 |
             CHAR_PTR_TO_UTF8 s[shift + 1] << 8  |
             CHAR_PTR_TO_UTF8 s[shift + 2];
    } else if ((s[shift] & 0xF8) == 0xF0) {
        ch = CHAR_PTR_TO_UTF8 s[shift + 0] << 24 |
             CHAR_PTR_TO_UTF8 s[shift + 1] << 16 |
             CHAR_PTR_TO_UTF8 s[shift + 2] << 8  |
             CHAR_PTR_TO_UTF8 s[shift + 3];
    } else {
        ch = 0xFFFFFFFF;
    }

    return u8tou32(ch);
}
