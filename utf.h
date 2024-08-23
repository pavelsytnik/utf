#ifndef UTF_H
#define UTF_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "chartypes.h"

#define UTF_CODEPOINT_MAX 0x10FFFFu

#define UTF_LEAD_SURROGATE_MIN 0xD800u
#define UTF_LEAD_SURROGATE_MAX 0xDBFFu
#define UTF_TRAIL_SURROGATE_MIN 0xDC00u
#define UTF_TRAIL_SURROGATE_MAX 0xDFFFu

#define UTF_IS_TRAIL(a) \
    (((a) & 0xC0) == 0x80)

#define UTF_IS_LEAD_SURROGATE(a) \
    ((a) >= UTF_LEAD_SURROGATE_MIN && (a) <= UTF_LEAD_SURROGATE_MAX)

#define UTF_IS_TRAIL_SURROGATE(a) \
    ((a) >= UTF_TRAIL_SURROGATE_MIN && (a) <= UTF_TRAIL_SURROGATE_MAX)

#define UTF_IS_SURROGATE(a) \
    ((a) >= UTF_LEAD_SURROGATE_MIN && (a) <= UTF_TRAIL_SURROGATE_MAX)

#define UTF_IS_VALID_CODEPOINT(a) \
    ((a) <= UTF_CODEPOINT_MAX && !UTF_IS_SURROGATE(a))

enum utf_error {
    UTF_OK = 0,
    UTF_INVALID_LEAD,
    UTF_INVALID_TRAIL,
    UTF_NOT_ENOUGH_ROOM,
    UTF_OVERLONG_SEQUENCE,
    UTF_INVALID_CODEPOINT
};

enum utf_endianness {
    UTF_LITTLE_ENDIAN = 1,
    UTF_BIG_ENDIAN = 2
};

char32_t u8_strget(const char8_t *s, size_t i);

size_t u8_strlen(const char8_t *s);
size_t u16_strlen(const char16_t *s);
size_t u32_strlen(const char32_t *s);

size_t u8_strlen_s(const char8_t *s, size_t n);
size_t u16_strlen_s(const char16_t *s, size_t n);
size_t u32_strlen_s(const char32_t *s, size_t n);

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
int utf_u8getc(char8_t *bytes, FILE *stream);

#endif /* UTF_H */
