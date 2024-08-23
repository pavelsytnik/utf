#include "utf.h"

void utf_str8to32(char32_t *restrict dst, const char8_t *restrict src)
{
    while (*src != 0) {
        char32_t ch = 0;
        if ((*src & 0x80) == 0x00) {
            ch |=  *src++;
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

void utf_str16to32(char32_t *restrict dst, const char16_t *restrict src)
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

void utf_str32to8(char8_t *restrict dst, const char32_t *restrict src)
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

void utf_str32to16(char16_t *restrict dst, const char32_t *restrict src)
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

const char8_t *utf_str8to32_s(char32_t *restrict dst,
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
