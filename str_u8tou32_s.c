#include "utf.h"

const char8_t *str_u8tou32_s(char32_t *restrict dst,
                             const char8_t *restrict src,
                             enum utf_error *stat)
{
    *stat = UTF_OK;

    while (*src != 0) {
        char32_t ch = 0;
        int seqlen = 0;
        
        // READ START BYTE OF SEQUENCE
        if ((*src & 0x80) == 0x00) {
            ch |= *src++;
            seqlen = 1;
        } else if ((*src & 0xE0) == 0xC0) {
            ch |= *src++ & 0x1F;
            seqlen = 2;
        } else if ((*src & 0xF0) == 0xE0) {
            ch |= *src++ & 0x0F;
            seqlen = 3;
        } else if ((*src & 0xF8) == 0xF0) {
            ch |= *src++ & 0x07;
            seqlen = 4;
        } else {
            *stat = UTF_INVALID_LEAD;
            break;
        }

        // READ CONTINUATION BYTES
        for (int i = 0; i < seqlen - 1; ++i) {
            if ((*src & 0xC0) == 0x80) {
                ch = ch << 6 | *src++ & 0x3F;
            } else if (*src == 0) {
                *stat = UTF_NOT_ENOUGH_ROOM;
                break;
            } else {
                *stat = UTF_INVALID_TRAIL;
                break;
            }
        }
        // CHARACTER HAS BEEN READ

        if (ch <= 0x7F   && seqlen > 1 ||
            ch <= 0x7FF  && seqlen > 2 ||
            ch <= 0xFFFF && seqlen > 3)
        {
            *stat = UTF_OVERLONG_SEQUENCE;
            src -= seqlen;
            break;
        }

        if (ch > 0x10FFFF) {
            *stat = UTF_INVALID_CODEPOINT;
            src -= 4;
            break;
        }
        
        *dst++ = ch;
    }

    *dst = 0;
    return src;
}
