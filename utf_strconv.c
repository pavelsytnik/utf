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
                              size_t n,
                              enum utf_error *stat)
{
    if (dst == NULL || src == NULL) {
        *stat = UTF_NOT_ENOUGH_ROOM;
        return NULL;
    }

    *stat = UTF_OK;

    if (stat == NULL)
        return NULL;

    while (*src != 0 && n-- > 0) {
        uint32_t cp;
        enum utf_error err = utf_validate_next(&src, &cp);
        if (err != UTF_OK) {
            *stat = err;
            break;
        }
        *dst++ = cp;
    }

    *dst = 0;
    return src;
}
