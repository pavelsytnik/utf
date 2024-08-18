#ifndef UTF_H
#define UTF_H

#include <stddef.h>
#include <stdint.h>

typedef uint32_t utf8_t, utf16_t, utf32_t;

utf8_t u32tou8(utf32_t c);
utf32_t u8tou32(utf8_t c);
utf16_t u32tou16(utf32_t c);
utf32_t u16tou32(utf16_t c);
utf32_t u8_strget(const char *s, size_t i);
size_t u8_strlen(const char *s);

#endif /* UTF_H */
