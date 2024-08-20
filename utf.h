#ifndef UTF_H
#define UTF_H

#include <stddef.h>
#include <stdint.h>

#include "chartypes.h"

#define u8_iscontbyte(a) (((a) & 0xC0) == 0x80)
#define u16_issurrogate(a) ((a) >= 0xD800 && (a) <= 0xDFFF)

#define isunicoderange(a) ((char32_t) (a) <= 0x10FFFF)
#define isvalidunicode(a) (!u16_issurrogate(a) && isunicoderange(a))

typedef uint32_t utf8_t, utf16_t, utf32_t;

utf8_t u32tou8(utf32_t c);
utf32_t u8tou32(utf8_t c);
utf16_t u32tou16(utf32_t c);
utf32_t u16tou32(utf16_t c);
utf32_t u8_strget(const char8_t *s, size_t i);
size_t u8_strlen(const char8_t *s);
size_t u16_strlen(const char16_t *s);
size_t u32_strlen(const char32_t *s);
void str_u8tou32(char32_t *restrict dst, const char8_t *restrict src);
void str_u32tou8(char8_t *restrict dst, const char32_t *restrict src);
void str_u32tou16(char16_t *restrict dst, const char32_t *restrict src);
void str_u16tou32(char32_t *restrict dst, const char16_t *restrict src);
char8_t *u8_strat(const char8_t *s, size_t i);

#endif /* UTF_H */
