#ifndef UTF_H
#define UTF_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "utf_char.h"

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

char32_t utf_str8get(const char8_t *s, size_t i);

size_t utf_str8len(const char8_t *s);
size_t utf_str16len(const char16_t *s);
size_t utf_str32len(const char32_t *s);

size_t utf_str8len_s(const char8_t *s, size_t n);
size_t utf_str16len_s(const char16_t *s, size_t n);
size_t utf_str32len_s(const char32_t *s, size_t n);

void utf_str8to32(char32_t *restrict dst, const char8_t *restrict src);
void utf_str32to8(char8_t *restrict dst, const char32_t *restrict src);
void utf_str32to16(char16_t *restrict dst, const char32_t *restrict src);
void utf_str16to32(char32_t *restrict dst, const char16_t *restrict src);
char8_t *utf_str8at(const char8_t *s, size_t i);

const char8_t *utf_str8to32_s(char32_t *restrict dst,
                              const char8_t *restrict src,
                              size_t n,
                              enum utf_error *stat);

enum utf_endianness utf_receive_endianness(void);

uint16_t utf_swapbytes_uint16(uint16_t n);
uint32_t utf_swapbytes_uint32(uint32_t n);

char8_t *utf_u8fread(char8_t *buf, size_t count, FILE *stream);
int utf_u8getc(char8_t *bytes, FILE *stream);

// For internal usage
enum utf_error utf_u8next(const char8_t **strp, uint32_t *codepoint);
enum utf_error utf_u16next(const char16_t **strp, uint32_t *codepoint);

#endif /* UTF_H */
