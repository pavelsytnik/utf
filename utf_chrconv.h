#ifndef utf_chrconv_h_
#define utf_chrconv_h_

#include "utf_byteorder.h"
#include "utf_char.h"
#include "utf_lang.h"

#ifdef UTF_INLINE /* Interface */

static UTF_INLINE
void utf_8_chr_to_16_be(const utf_c8  *UTF_RESTRICT src,
                              utf_c16 *UTF_RESTRICT dst);
static UTF_INLINE
void utf_8_chr_to_16_le(const utf_c8  *UTF_RESTRICT src,
                              utf_c16 *UTF_RESTRICT dst);
static UTF_INLINE
void utf_8_chr_to_32_be(const utf_c8  *UTF_RESTRICT src,
                              utf_c32 *UTF_RESTRICT dst);
static UTF_INLINE
void utf_8_chr_to_32_le(const utf_c8  *UTF_RESTRICT src,
                              utf_c32 *UTF_RESTRICT dst);
static UTF_INLINE
void utf_16_be_chr_to_8(const utf_c16 *UTF_RESTRICT src,
                              utf_c8  *UTF_RESTRICT dst);
static UTF_INLINE
void utf_16_be_chr_to_32_be(const utf_c16 *UTF_RESTRICT src,
                                  utf_c32 *UTF_RESTRICT dst);
static UTF_INLINE
void utf_16_be_chr_to_32_le(const utf_c16 *UTF_RESTRICT src,
                                  utf_c32 *UTF_RESTRICT dst);
static UTF_INLINE
void utf_16_le_chr_to_8(const utf_c16 *UTF_RESTRICT src,
                              utf_c8  *UTF_RESTRICT dst);
static UTF_INLINE
void utf_16_le_chr_to_32_be(const utf_c16 *UTF_RESTRICT src,
                                  utf_c32 *UTF_RESTRICT dst);
static UTF_INLINE
void utf_16_le_chr_to_32_le(const utf_c16 *UTF_RESTRICT src,
                                  utf_c32 *UTF_RESTRICT dst);
static UTF_INLINE
void utf_32_be_chr_to_8(const utf_c32 *UTF_RESTRICT src,
                              utf_c8  *UTF_RESTRICT dst);
static UTF_INLINE
void utf_32_be_chr_to_16_be(const utf_c32 *UTF_RESTRICT src,
                                  utf_c16 *UTF_RESTRICT dst);
static UTF_INLINE
void utf_32_be_chr_to_16_le(const utf_c32 *UTF_RESTRICT src,
                                  utf_c16 *UTF_RESTRICT dst);
static UTF_INLINE
void utf_32_le_chr_to_8(const utf_c32 *UTF_RESTRICT src,
                              utf_c8  *UTF_RESTRICT dst);
static UTF_INLINE
void utf_32_le_chr_to_16_be(const utf_c32 *UTF_RESTRICT src,
                                  utf_c16 *UTF_RESTRICT dst);
static UTF_INLINE
void utf_32_le_chr_to_16_le(const utf_c32 *UTF_RESTRICT src,
                                  utf_c16 *UTF_RESTRICT dst);
static UTF_INLINE
void utf_16_be_chr_to_le(const utf_c16 *src, utf_c16 *dst);
static UTF_INLINE
void utf_16_le_chr_to_be(const utf_c16 *src, utf_c16 *dst);
static UTF_INLINE
void utf_32_chr_to_opposite(const utf_c32 *src, utf_c32 *dst);

#else /* Interface */

#if defined UTF_BIG_ENDIAN
#    define utf_8_chr_to_16_be(src, dst) \
         utf_8_chr_to_16_native_(src, dst)
#    define utf_8_chr_to_16_le(src, dst) \
         utf_8_chr_to_16_foreign_(src, dst)
#    define utf_8_chr_to_32_be(src, dst) \
         utf_8_chr_to_32_(src, dst)
#    define utf_8_chr_to_32_le(src, dst) \
         utf_8_chr_to_32_foreign_(src, dst)
#    define utf_16_be_chr_to_8(src, dst) \
         utf_16_native_chr_to_8_(src, dst)
#    define utf_16_be_chr_to_le(src, dst) \
         utf_16_chr_to_foreign_(src, dst)
#    define utf_16_be_chr_to_32_be(src, dst) \
         utf_16_chr_to_32_(src, dst)
#    define utf_16_be_chr_to_32_le(src, dst) \
         utf_16_native_chr_to_32_foreign_(src, dst)
#    define utf_16_le_chr_to_8(src, dst) \
         utf_16_foreign_chr_to_8_(src, dst)
#    define utf_16_le_chr_to_be(src, dst) \
         utf_16_chr_to_native_(src, dst)
#    define utf_16_le_chr_to_32_be(src, dst) \
         utf_16_foreign_chr_to_32_native_(src, dst)
#    define utf_16_le_chr_to_32_le(src, dst) \
         utf_16_foreign_chr_to_32_foreign_(src, dst)
#    define utf_32_be_chr_to_8(src, dst) \
         utf_32_chr_to_8_(src, dst)
#    define utf_32_be_chr_to_16_be(src, dst) \
         utf_32_chr_to_16_(src, dst)
#    define utf_32_be_chr_to_16_le(src, dst) \
         utf_32_native_chr_to_16_foreign_(src, dst)
#    define utf_32_le_chr_to_8(src, dst) \
         utf_32_foreign_chr_to_8_(src, dst)
#    define utf_32_le_chr_to_16_be(src, dst) \
         utf_32_foreign_chr_to_16_native_(src, dst)
#    define utf_32_le_chr_to_16_le(src, dst) \
         utf_32_foreign_chr_to_16_foreign_(src, dst)
#elif defined UTF_LITTLE_ENDIAN
#    define utf_8_chr_to_16_be(src, dst) \
         utf_8_chr_to_16_foreign_(src, dst)
#    define utf_8_chr_to_16_le(src, dst) \
         utf_8_chr_to_16_native_(src, dst)
#    define utf_8_chr_to_32_be(src, dst) \
         utf_8_chr_to_32_foreign_(src, dst)
#    define utf_8_chr_to_32_le(src, dst) \
         utf_8_chr_to_32_(src, dst)
#    define utf_16_be_chr_to_8(src, dst) \
         utf_16_foreign_chr_to_8_(src, dst)
#    define utf_16_be_chr_to_le(src, dst) \
         utf_16_chr_to_native_(src, dst)
#    define utf_16_be_chr_to_32_be(src, dst) \
         utf_16_foreign_chr_to_32_foreign_(src, dst)
#    define utf_16_be_chr_to_32_le(src, dst) \
         utf_16_foreign_chr_to_32_native_(src, dst)
#    define utf_16_le_chr_to_8(src, dst) \
         utf_16_native_chr_to_8_(src, dst)
#    define utf_16_le_chr_to_be(src, dst) \
         utf_16_chr_to_foreign_(src, dst)
#    define utf_16_le_chr_to_32_be(src, dst) \
         utf_16_native_chr_to_32_foreign_(src, dst)
#    define utf_16_le_chr_to_32_le(src, dst) \
         utf_16_chr_to_32_(src, dst)
#    define utf_32_be_chr_to_8(src, dst) \
         utf_32_foreign_chr_to_8_(src, dst)
#    define utf_32_be_chr_to_16_be(src, dst) \
         utf_32_foreign_chr_to_16_foreign_(src, dst)
#    define utf_32_be_chr_to_16_le(src, dst) \
         utf_32_foreign_chr_to_16_native_(src, dst)
#    define utf_32_le_chr_to_8(src, dst) \
         utf_32_chr_to_8_(src, dst)
#    define utf_32_le_chr_to_16_be(src, dst) \
         utf_32_native_chr_to_16_foreign_(src, dst)
#    define utf_32_le_chr_to_16_le(src, dst) \
         utf_32_chr_to_16_(src, dst)
#endif

#define utf_32_chr_to_opposite(src, dst) \
    do { *(dst) = utf_c32_bswap(*(src)); } while (0)

#endif /* Interface */

#ifdef UTF_INLINE /* Algorithms */

static UTF_INLINE
void utf_8_chr_to_32_(const utf_c8  *UTF_RESTRICT src,
                            utf_c32 *UTF_RESTRICT dst);
static UTF_INLINE
void utf_32_chr_to_8_(const utf_c32 *UTF_RESTRICT src,
                            utf_c8  *UTF_RESTRICT dst);
static UTF_INLINE
void utf_16_chr_to_32_(const utf_c16 *UTF_RESTRICT src,
                             utf_c32 *UTF_RESTRICT dst);
static UTF_INLINE
void utf_32_chr_to_16_(const utf_c32 *UTF_RESTRICT src,
                             utf_c16 *UTF_RESTRICT dst);

#else /* Algorithms */

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

#endif /* Algorithms */

#define utf_16_chr_to_foreign_(src, dst) do \
{                                           \
    if (utf_is_surrogate(src[0])) {         \
        dst[0] = utf_c16_bswap(src[0]);     \
        dst[1] = utf_c16_bswap(src[1]);     \
    } else {                                \
        dst[0] = utf_c16_bswap(src[0]);     \
    }                                       \
}                                           \
while (0)

#define utf_16_chr_to_native_(src, dst) do         \
{                                                  \
    if (utf_is_surrogate(utf_c16_bswap(src[0]))) { \
        dst[0] = utf_c16_bswap(src[0]);            \
        dst[1] = utf_c16_bswap(src[1]);            \
    } else {                                       \
        dst[0] = utf_c16_bswap(src[0]);            \
    }                                              \
}                                                  \
while (0)

#define utf_8_chr_to_16_native_(src, dst) do \
{                                            \
    utf_c32 cp;                              \
    utf_8_chr_to_32_(src, &cp);              \
    utf_32_chr_to_16_(&cp, dst);             \
}                                            \
while (0)

#define utf_8_chr_to_16_foreign_(src, dst) do \
{                                             \
    utf_c32 cp;                               \
    utf_8_chr_to_32_(src, &cp);               \
    utf_32_chr_to_16_(&cp, dst);              \
    utf_16_chr_to_foreign_(dst, dst);         \
}                                             \
while (0)

#define utf_8_chr_to_32_foreign_(src, dst) do \
{                                             \
    utf_8_chr_to_32_(src, dst);               \
    utf_32_chr_to_opposite(dst, dst);         \
}                                             \
while (0)

#define utf_16_native_chr_to_8_(src, dst) do \
{                                            \
    utf_c32 cp;                              \
    utf_16_chr_to_32_(src, &cp);             \
    utf_32_chr_to_8_(&cp, dst);              \
}                                            \
while (0)

#define utf_16_native_chr_to_32_foreign_(src, dst) do \
{                                                     \
    utf_16_chr_to_32_(src, dst);                      \
    utf_32_chr_to_opposite(dst, dst);                 \
}                                                     \
while (0)

#define utf_16_foreign_chr_to_8_(src, dst) do \
{                                             \
    utf_c32 cp;                               \
    utf_c16 swapped[2];                       \
    utf_16_chr_to_native_(src, swapped);      \
    utf_16_chr_to_32_(swapped, &cp);          \
    utf_32_chr_to_8_(&cp, dst);               \
}                                             \
while (0)

#define utf_16_foreign_chr_to_32_native_(src, dst) do \
{                                                     \
    utf_c16 swapped[2];                               \
    utf_16_chr_to_native_(src, swapped);              \
    utf_16_chr_to_32_(swapped, dst);                  \
}                                                     \
while (0)

#define utf_16_foreign_chr_to_32_foreign_(src, dst) do \
{                                                      \
    utf_c16 swapped[2];                                \
    utf_16_chr_to_native_(src, swapped);               \
    utf_16_chr_to_32_(swapped, dst);                   \
    utf_32_chr_to_opposite(dst, dst);                  \
}                                                      \
while (0)

#define utf_32_native_chr_to_16_foreign_(src, dst) do \
{                                                     \
    utf_32_chr_to_16_(src, dst);                      \
    utf_16_chr_to_foreign_(dst, dst);                 \
}                                                     \
while (0)

#define utf_32_foreign_chr_to_8_(src, dst) do \
{                                             \
    utf_c32 swapped[1];                       \
    utf_32_chr_to_opposite(src, swapped);     \
    utf_32_chr_to_8_(swapped, dst);           \
}                                             \
while (0)

#define utf_32_foreign_chr_to_16_native_(src, dst) do \
{                                                     \
    utf_c32 swapped[1];                               \
    utf_32_chr_to_opposite(src, swapped);             \
    utf_32_chr_to_16_(swapped, dst);                  \
}                                                     \
while (0)

#define utf_32_foreign_chr_to_16_foreign_(src, dst) do \
{                                                      \
    utf_c32 swapped[1];                                \
    utf_32_chr_to_opposite(src, swapped);              \
    utf_32_chr_to_16_(swapped, dst);                   \
    utf_16_chr_to_foreign_(dst, dst);                  \
}                                                      \
while (0)

#ifdef UTF_INLINE /* Function definitions */

static UTF_INLINE
void utf_8_chr_to_32_(const utf_c8  *UTF_RESTRICT src,
                            utf_c32 *UTF_RESTRICT dst)
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
void utf_32_chr_to_8_(const utf_c32 *UTF_RESTRICT src,
                            utf_c8  *UTF_RESTRICT dst)
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
void utf_16_chr_to_32_(const utf_c16 *UTF_RESTRICT src,
                             utf_c32 *UTF_RESTRICT dst)
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
void utf_32_chr_to_16_(const utf_c32 *UTF_RESTRICT src,
                             utf_c16 *UTF_RESTRICT dst)
{
    if (*src < 0x10000) {
        *dst = *src;
    } else if (*src < 0x110000) {
        utf_c32 tmp = *src - 0x10000;
        *dst++ = (tmp >> 10)   + 0xD800;
        *dst   = (tmp & 0x3FF) + 0xDC00;
    }
}

static UTF_INLINE
void utf_8_chr_to_16_be(const utf_c8  *UTF_RESTRICT src,
                              utf_c16 *UTF_RESTRICT dst)
{
#if defined UTF_BIG_ENDIAN
    utf_8_chr_to_16_native_(src, dst);
#elif defined UTF_LITTLE_ENDIAN
    utf_8_chr_to_16_foreign_(src, dst);
#endif
}

static UTF_INLINE
void utf_8_chr_to_16_le(const utf_c8  *UTF_RESTRICT src,
                              utf_c16 *UTF_RESTRICT dst)
{
#if defined UTF_BIG_ENDIAN
    utf_8_chr_to_16_foreign_(src, dst);
#elif defined UTF_LITTLE_ENDIAN
    utf_8_chr_to_16_native_(src, dst);
#endif
}

static UTF_INLINE
void utf_8_chr_to_32_be(const utf_c8  *UTF_RESTRICT src,
                              utf_c32 *UTF_RESTRICT dst)
{
#if defined UTF_BIG_ENDIAN
    utf_8_chr_to_32_(src, dst);
#elif defined UTF_LITTLE_ENDIAN
    utf_8_chr_to_32_foreign_(src, dst);
#endif
}

static UTF_INLINE
void utf_8_chr_to_32_le(const utf_c8  *UTF_RESTRICT src,
                              utf_c32 *UTF_RESTRICT dst)
{
#if defined UTF_BIG_ENDIAN
    utf_8_chr_to_32_foreign_(src, dst);
#elif defined UTF_LITTLE_ENDIAN
    utf_8_chr_to_32_(src, dst);
#endif
}

static UTF_INLINE
void utf_16_be_chr_to_8(const utf_c16 *UTF_RESTRICT src,
                              utf_c8  *UTF_RESTRICT dst)
{
#if defined UTF_BIG_ENDIAN
    utf_16_native_chr_to_8_(src, dst);
#elif defined UTF_LITTLE_ENDIAN
    utf_16_foreign_chr_to_8_(src, dst);
#endif
}

static UTF_INLINE
void utf_16_be_chr_to_32_be(const utf_c16 *UTF_RESTRICT src,
                                  utf_c32 *UTF_RESTRICT dst)
{
#if defined UTF_BIG_ENDIAN
    utf_16_chr_to_32_(src, dst);
#elif defined UTF_LITTLE_ENDIAN
    utf_16_foreign_chr_to_32_foreign_(src, dst);
#endif
}

static UTF_INLINE
void utf_16_be_chr_to_32_le(const utf_c16 *UTF_RESTRICT src,
                                  utf_c32 *UTF_RESTRICT dst)
{
#if defined UTF_BIG_ENDIAN
    utf_16_native_chr_to_32_foreign_(src, dst);
#elif defined UTF_LITTLE_ENDIAN
    utf_16_foreign_chr_to_32_native_(src, dst);
#endif
}

static UTF_INLINE
void utf_16_le_chr_to_8(const utf_c16 *UTF_RESTRICT src,
                              utf_c8  *UTF_RESTRICT dst)
{
#if defined UTF_BIG_ENDIAN
    utf_16_foreign_chr_to_8_(src, dst);
#elif defined UTF_LITTLE_ENDIAN
    utf_16_native_chr_to_8_(src, dst);
#endif
}

static UTF_INLINE
void utf_16_le_chr_to_32_be(const utf_c16 *UTF_RESTRICT src,
                                  utf_c32 *UTF_RESTRICT dst)
{
#if defined UTF_BIG_ENDIAN
    utf_16_foreign_chr_to_32_native_(src, dst);
#elif defined UTF_LITTLE_ENDIAN
    utf_16_native_chr_to_32_foreign_(src, dst);
#endif
}

static UTF_INLINE
void utf_16_le_chr_to_32_le(const utf_c16 *UTF_RESTRICT src,
                                  utf_c32 *UTF_RESTRICT dst)
{
#if defined UTF_BIG_ENDIAN
    utf_16_foreign_chr_to_32_foreign_(src, dst);
#elif defined UTF_LITTLE_ENDIAN
    utf_16_chr_to_32_(src, dst);
#endif
}

static UTF_INLINE
void utf_32_be_chr_to_8(const utf_c32 *UTF_RESTRICT src,
                              utf_c8  *UTF_RESTRICT dst)
{
#if defined UTF_BIG_ENDIAN
    utf_32_chr_to_8_(src, dst);
#elif defined UTF_LITTLE_ENDIAN
    utf_32_foreign_chr_to_8_(src, dst);
#endif
}

static UTF_INLINE
void utf_32_be_chr_to_16_be(const utf_c32 *UTF_RESTRICT src,
                                  utf_c16 *UTF_RESTRICT dst)
{
#if defined UTF_BIG_ENDIAN
    utf_32_chr_to_16_(src, dst);
#elif defined UTF_LITTLE_ENDIAN
    utf_32_foreign_chr_to_16_foreign_(src, dst);
#endif
}

static UTF_INLINE
void utf_32_be_chr_to_16_le(const utf_c32 *UTF_RESTRICT src,
                                  utf_c16 *UTF_RESTRICT dst)
{
#if defined UTF_BIG_ENDIAN
    utf_32_native_chr_to_16_foreign_(src, dst);
#elif defined UTF_LITTLE_ENDIAN
    utf_32_foreign_chr_to_16_native_(src, dst);
#endif
}

static UTF_INLINE
void utf_32_le_chr_to_8(const utf_c32 *UTF_RESTRICT src,
                              utf_c8  *UTF_RESTRICT dst)
{
#if defined UTF_BIG_ENDIAN
    utf_32_foreign_chr_to_8_(src, dst);
#elif defined UTF_LITTLE_ENDIAN
    utf_32_chr_to_8_(src, dst);
#endif
}

static UTF_INLINE
void utf_32_le_chr_to_16_be(const utf_c32 *UTF_RESTRICT src,
                                  utf_c16 *UTF_RESTRICT dst)
{
#if defined UTF_BIG_ENDIAN
    utf_32_foreign_chr_to_16_native_(src, dst);
#elif defined UTF_LITTLE_ENDIAN
    utf_32_native_chr_to_16_foreign_(src, dst);
#endif
}

static UTF_INLINE
void utf_32_le_chr_to_16_le(const utf_c32 *UTF_RESTRICT src,
                                  utf_c16 *UTF_RESTRICT dst)
{
#if defined UTF_BIG_ENDIAN
    utf_32_foreign_chr_to_16_foreign_(src, dst);
#elif defined UTF_LITTLE_ENDIAN
    utf_32_chr_to_16_(src, dst);
#endif
}

static UTF_INLINE
void utf_16_be_chr_to_le(const utf_c16 *src, utf_c16 *dst)
{
#if defined UTF_BIG_ENDIAN
    utf_16_chr_to_foreign_(src, dst);
#elif defined UTF_LITTLE_ENDIAN
    utf_16_chr_to_native_(src, dst);
#endif
}

static UTF_INLINE
void utf_16_le_chr_to_be(const utf_c16 *src, utf_c16 *dst)
{
#if defined UTF_BIG_ENDIAN
    utf_16_chr_to_native_(src, dst);
#elif defined UTF_LITTLE_ENDIAN
    utf_16_chr_to_foreign_(src, dst);
#endif
}

static UTF_INLINE
void utf_32_chr_to_opposite(const utf_c32 *src, utf_c32 *dst)
{
    *dst = utf_c32_bswap(*src);
}

#endif /* Function definitions */

#endif /* !defined(utf_chrconv_h_) */
