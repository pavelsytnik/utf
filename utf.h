#ifndef UTF_H
#define UTF_H

#include <stddef.h>
#include <stdint.h>

#include "chartypes.h"

#define u8_iscontbyte(a) (((a) & 0xC0) == 0x80)
#define u16_issurrogate(a) ((a) >= 0xD800 && (a) <= 0xDFFF)

typedef uint32_t utf8_t, utf16_t, utf32_t;

utf8_t u32tou8(utf32_t c);
utf32_t u8tou32(utf8_t c);
utf16_t u32tou16(utf32_t c);
utf32_t u16tou32(utf16_t c);
utf32_t u8_strget(const char8_t *s, size_t i);
size_t u8_strlen(const char8_t *s);
size_t u16_strlen(const char16_t *s);
size_t u32_strlen(const char32_t *s);

#endif /* UTF_H */
