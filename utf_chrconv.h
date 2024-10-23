/* The header is for internal usage only */

#ifndef utf_chrconv_h_
#define utf_chrconv_h_

#include "utf_char.h"
#include "utf_lang.h"

#include <stddef.h>

#ifdef UTF_INLINE

static UTF_INLINE
void utf_8_chr_to_32_(const utf_c8 *restrict src, utf_c32 *restrict dst);
static UTF_INLINE
void utf_16_chr_to_32_(const utf_c16 *restrict src, utf_c32 *restrict dst);
static UTF_INLINE
void utf_8_chr_to_16_(const utf_c8 *restrict src, utf_c16 *restrict dst);
static UTF_INLINE
void utf_16_chr_to_8_(const utf_c16 *restrict src, utf_c8 *restrict dst);
static UTF_INLINE
void utf_32_chr_to_8_(const utf_c32 *restrict src, utf_c8 *restrict dst);
static UTF_INLINE
void utf_32_chr_to_16_(const utf_c32 *restrict src, utf_c16 *restrict dst);

static UTF_INLINE
void utf_8_chr_to_32_(const utf_c8 *restrict src, utf_c32 *restrict dst)
{
    if (utf_8_is_lead_1(*src)) {
        *dst = *src;
    } else if (utf_8_is_lead_2(*src)) {
        *dst  = (*src++ & 0x1F) << 6;
        *dst |= (*src   & 0x3F);
    } else if (utf_8_is_lead_3(*src)) {
        *dst  = (*src++ & 0x0F) << 12;
        *dst |= (*src++ & 0x3F) << 6;
        *dst |= (*src   & 0x3F);
    } else if (utf_8_is_lead_4(*src)) {
        *dst  = (*src++ & 0x07) << 18;
        *dst |= (*src++ & 0x3F) << 12;
        *dst |= (*src++ & 0x3F) << 6;
        *dst |= (*src   & 0x3F);
    }
}

static UTF_INLINE
void utf_32_chr_to_8_(const utf_c32 *restrict src, utf_c8 *restrict dst)
{
    if (*src < 0x80) {
        *dst = *src;
    } else if (*src < 0x800) {
        *dst++ = *src >> 6   | 0xC0;
        *dst   = *src & 0x3F | 0x80;
    } else if (*src < 0x10000) {
        *dst++ = *src >> 12        | 0xE0;
        *dst++ = *src >> 6  & 0x3F | 0x80;
        *dst   = *src       & 0x3F | 0x80;
    } else if (*src < 0x110000) {
        *dst++ = *src >> 18        | 0xF0;
        *dst++ = *src >> 12 & 0x3F | 0x80;
        *dst++ = *src >> 6  & 0x3F | 0x80;
        *dst   = *src       & 0x3F | 0x80;
    }
}

static UTF_INLINE
void utf_16_chr_to_32_(const utf_c16 *restrict src, utf_c32 *restrict dst)
{
    if (!utf_is_surrogate(*src)) {
        *dst = *src;
    } else if (utf_is_lead_surrogate(*src)) {
        *dst  = *src++ - 0xD800 << 10;
        *dst |= *src   - 0xDC00;
        *dst += 0x10000;
    }
}

static UTF_INLINE
void utf_32_chr_to_16_(const utf_c32 *restrict src, utf_c16 *restrict dst)
{
    if (*src < 0x10000) {
        *dst = *src;
    } else if (*src < 0x110000) {
        utf_c32 tmp = *src - 0x10000;
        *dst++ = (tmp >> 10)   + 0xD800;
        *dst =   (tmp & 0x3FF) + 0xDC00;
    }
}

static UTF_INLINE
void utf_8_chr_to_16_(const utf_c8 *restrict src, utf_c16 *restrict dst)
{
    utf_c32 cp;
    utf_8_chr_to_32_(src, &cp);
    utf_32_chr_to_16_(&cp, dst);
}

static UTF_INLINE
void utf_16_chr_to_8_(const utf_c16 *restrict src, utf_c8 *restrict dst)
{
    utf_c32 cp;
    utf_16_chr_to_32_(src, &cp);
    utf_32_chr_to_8_(&cp, dst);
}

#else /* ifndef UTF_INLINE */

#define utf_8_chr_to_32_(src, dst) do      \
{                                          \
    if (utf_8_is_lead_1(*(src))) {         \
        *(dst) = *(src);                   \
    } else if (utf_8_is_lead_2(*(src))) {  \
        *(dst)  = ((src)[0] & 0x1F) << 6;  \
        *(dst) |= ((src)[1] & 0x3F);       \
    } else if (utf_8_is_lead_3(*(src))) {  \
        *(dst)  = ((src)[0] & 0x0F) << 12; \
        *(dst) |= ((src)[1] & 0x3F) << 6;  \
        *(dst) |= ((src)[2] & 0x3F);       \
    } else if (utf_8_is_lead_4(*(src))) {  \
        *(dst)  = ((src)[0] & 0x07) << 18; \
        *(dst) |= ((src)[1] & 0x3F) << 12; \
        *(dst) |= ((src)[2] & 0x3F) << 6;  \
        *(dst) |= ((src)[3] & 0x3F);       \
    }                                      \
}                                          \
while (0)

#define utf_32_chr_to_8_(src, dst) do          \
{                                              \
    if (*(src) < 0x80) {                       \
        *(dst) = *(src);                       \
    } else if (*(src) < 0x800) {               \
        (dst)[0] = *(src) >> 6   | 0xC0;       \
        (dst)[1] = *(src) & 0x3F | 0x80;       \
    } else if (*(src) < 0x10000) {             \
        (dst)[0] = *(src) >> 12        | 0xE0; \
        (dst)[1] = *(src) >> 6  & 0x3F | 0x80; \
        (dst)[2] = *(src)       & 0x3F | 0x80; \
    } else if (*(src) < 0x110000) {            \
        (dst)[0] = *(src) >> 18        | 0xF0; \
        (dst)[1] = *(src) >> 12 & 0x3F | 0x80; \
        (dst)[2] = *(src) >> 6  & 0x3F | 0x80; \
        (dst)[3] = *(src)       & 0x3F | 0x80; \
    }                                          \
}                                              \
while (0)

#define utf_16_chr_to_32_(src, dst) do          \
{                                               \
    if (!utf_is_surrogate(*(src))) {            \
        *(dst) = *(src);                        \
    } else if (utf_is_lead_surrogate(*(src))) { \
        *(dst)  = (src)[0] - 0xD800 << 10;      \
        *(dst) |= (src)[1] - 0xDC00;            \
        *(dst) += 0x10000;                      \
    }                                           \
}                                               \
while (0)

#define utf_32_chr_to_16_(src, dst) do     \
{                                          \
    if (*(src) < 0x10000) {                \
        *(dst) = *(src);                   \
    } else if (*(src) < 0x110000) {        \
        utf_c32 tmp = *(src) - 0x10000;    \
        (dst)[0] = (tmp >> 10)   + 0xD800; \
        (dst)[1] = (tmp & 0x3FF) + 0xDC00; \
    }                                      \
}                                          \
while (0)

#define utf_8_chr_to_16_(src, dst) do \
{                                     \
    utf_c32 cp;                       \
    utf_8_chr_to_32_(src, &cp);       \
    utf_32_chr_to_16_(&cp, dst);      \
}                                     \
while (0)

#define utf_16_chr_to_8_(src, dst) do \
{                                     \
    utf_c32 cp;                       \
    utf_16_chr_to_32_(src, &cp);      \
    utf_32_chr_to_8_(&cp, dst);       \
}                                     \
while (0)

#endif /* UTF_INLINE */

#endif /* !defined(utf_chrconv_h_) */
