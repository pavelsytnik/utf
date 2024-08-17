#ifndef UTF_H
#define UTF_H

#include <stddef.h>
#include <stdint.h>

typedef uint32_t utf8_t, utf16_t, utf32_t;

utf8_t u32tou8(utf32_t c);
size_t u8_strget(const char *s, size_t i);
size_t u8_strlen(const char *s);

#endif /* UTF_H */
