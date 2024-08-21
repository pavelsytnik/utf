#ifndef UTF_H
#define UTF_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "chartypes.h"

#define u8_iscontbyte(a) (((a) & 0xC0) == 0x80)
#define u16_issurrogate(a) ((a) >= 0xD800 && (a) <= 0xDFFF)

#define isunicoderange(a) ((char32_t) (a) <= 0x10FFFF)
#define isvalidunicode(a) (!u16_issurrogate(a) && isunicoderange(a))

enum utf_error {
    UTF_OK = 0,
    UTF_BAD_BYTE,
    UTF_UNEXPECTED_CONTINUATION,
    UTF_TRUNCATED,
    UTF_OVERLONG,
    UTF_BAD_CODEPOINT
};

enum utf_endianness {
    UTF_LITTLE_ENDIAN = 1,
    UTF_BIG_ENDIAN = 2
};

char32_t u8_strget(const char8_t *s, size_t i);
size_t u8_strlen(const char8_t *s);
size_t u16_strlen(const char16_t *s);
size_t u32_strlen(const char32_t *s);
void str_u8tou32(char32_t *restrict dst, const char8_t *restrict src);
void str_u32tou8(char8_t *restrict dst, const char32_t *restrict src);
void str_u32tou16(char16_t *restrict dst, const char32_t *restrict src);
void str_u16tou32(char32_t *restrict dst, const char16_t *restrict src);
char8_t *u8_strat(const char8_t *s, size_t i);

const char8_t *str_u8tou32_s(char32_t *restrict dst,
                             const char8_t *restrict src,
                             enum utf_error *stat);

enum utf_endianness utf_receive_endianness(void);

uint16_t utf_swapbytes_uint16(uint16_t n);
uint32_t utf_swapbytes_uint32(uint32_t n);

char8_t *utf_fu8read(char8_t *buf, size_t count, FILE *stream);

#endif /* UTF_H */
