#include "utf_strconv.h"

void utf_8_to_16(utf_c16 *restrict dst, const utf_c8 *restrict src)
{
    while (*src != 0) {
        uint32_t c32 = 0;
        if ((*src & 0x80) == 0x00) {
            c32 |= (*src++ & 0xFF) << 0;
        } else if ((*src & 0xE0) == 0xC0) {
            c32 |= (*src++ & 0x1F) << 6;
            c32 |= (*src++ & 0x3F) << 0;
        } else if ((*src & 0xF0) == 0xE0) {
            c32 |= (*src++ & 0x0F) << 12;
            c32 |= (*src++ & 0x3F) << 6;
            c32 |= (*src++ & 0x3F) << 0;
        } else if ((*src & 0xF8) == 0xF0) {
            c32 |= (*src++ & 0x07) << 18;
            c32 |= (*src++ & 0x3F) << 12;
            c32 |= (*src++ & 0x3F) << 6;
            c32 |= (*src++ & 0x3F) << 0;
        } else {
            break;
        }

        if (c32 < 0x10000) {
            *dst++ = c32;
        } else if (c32 < 0x110000) {
            c32 -= 0x10000;
            *dst++ = (c32 >> 10) + 0xD800;
            *dst++ = (c32 & 0x3FF) + 0xDC00;
        } else {
            break;
        }
    }

    *dst = 0;
}

void utf_8_to_32(utf_c32 *restrict dst, const utf_c8 *restrict src)
{
    while (*src != 0) {
        utf_c32 ch = 0;
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

void utf_16_to_8(utf_c8 *restrict dst, const utf_c16 *restrict src)
{
    while (*src != 0) {
        uint32_t c32 = 0;
        if (*src >= 0xD800 && *src <= 0xDBFF) {
            c32 |= *src++ - 0xD800 << 10;
            c32 |= *src++ - 0xDC00;
            c32 += 0x10000;
        } else if (*src < 0xDC00 || *src > 0xDFFF) {
            c32 |= *src++;
        } else {
            break;
        }

        if (c32 < 0x80) {
            *dst++ = c32;
        } else if (c32 < 0x800) {
            *dst++ = c32 >> 6 | 0xC0;
            *dst++ = c32 & 0x3F | 0x80;
        } else if (c32 < 0x10000) {
            *dst++ = c32 >> 12 | 0xE0;
            *dst++ = c32 >> 6 & 0x3F | 0x80;
            *dst++ = c32 & 0x3F | 0x80;
        } else if (c32 < 0x110000) {
            *dst++ = c32 >> 18 | 0xF0;
            *dst++ = c32 >> 12 & 0x3F | 0x80;
            *dst++ = c32 >> 6 & 0x3F | 0x80;
            *dst++ = c32 & 0x3F | 0x80;
        } else {
            break;
        }
    }

    *dst = 0;
}

void utf_16_to_32(utf_c32 *restrict dst, const utf_c16 *restrict src)
{
    while (*src != 0) {
        utf_c32 ch = 0;
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

void utf_32_to_8(utf_c8 *restrict dst, const utf_c32 *restrict src)
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

void utf_32_to_16(utf_c16 *restrict dst, const utf_c32 *restrict src)
{
    while (*src != 0) {
        if (*src < 0x10000) {
            *dst++ = *src;
        } else if (*src < 0x110000) {
            utf_c32 ch = *src - 0x10000;
            *dst++ = (ch >> 10) + 0xD800;
            *dst++ = (ch & 0x3FF) + 0xDC00;
        } else {
            break;
        }
        src++;
    }
    *dst = 0;
}

#define UTF_GENERATE_BODY_FOR_STRTO32FROM_(i)            \
    if (dst == NULL || src == NULL) {                    \
        *stat = UTF_TRUNCATED;                           \
        return NULL;                                     \
    }                                                    \
                                                         \
    *stat = UTF_OK;                                      \
                                                         \
    if (stat == NULL)                                    \
        return NULL;                                     \
                                                         \
    while (*src != 0 && n-- > 0) {                       \
        uint32_t cp;                                     \
        utf_error err = utf_##i##_next(&src, &cp);       \
        if (err != UTF_OK) {                             \
            *stat = err;                                 \
            break;                                       \
        }                                                \
        *dst++ = cp;                                     \
    }                                                    \
                                                         \
    *dst = 0;                                            \
    return src;

const utf_c8 *utf_8_to_32_s(utf_c32 *restrict dst,
                            const utf_c8 *restrict src,
                            size_t n,
                            utf_error *stat)
{
    UTF_GENERATE_BODY_FOR_STRTO32FROM_(8);
}

const utf_c16 *utf_16_to_32_s(utf_c32 *restrict dst,
                              const utf_c16 *restrict src,
                              size_t n,
                              utf_error *stat)
{
    UTF_GENERATE_BODY_FOR_STRTO32FROM_(16);
}

//static const utf_c8 *utf_strto32_s(utf_c32 *restrict dst,
//                                   const utf_c8 *restrict src,
//                                   size_t n,
//                                   enum utf_error *stat)
//{
//    if (dst == NULL || src == NULL) {
//        *stat = UTF_TRUNCATED;
//        return NULL;
//    }
//
//    *stat = UTF_OK;
//
//    if (stat == NULL)
//        return NULL;
//
//    while (*src != 0 && n-- > 0) {
//        uint32_t cp;
//        enum utf_error err = utf_8_next(&src, &cp);
//        if (err != UTF_OK) {
//            *stat = err;
//            break;
//        }
//        *dst++ = cp;
//    }
//
//    *dst = 0;
//    return src;
//}
