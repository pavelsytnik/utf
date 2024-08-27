#ifndef UTF_STRCONV_H
#define UTF_STRCONV_H

#include "utf_char.h"
#include "utf_error.h"

#include <stddef.h>

void utf_str8to32(char32_t *restrict dst, const char8_t *restrict src);
void utf_str32to8(char8_t *restrict dst, const char32_t *restrict src);
void utf_str32to16(char16_t *restrict dst, const char32_t *restrict src);
void utf_str16to32(char32_t *restrict dst, const char16_t *restrict src);

const char8_t *utf_str8to32_s(char32_t *restrict dst,
                              const char8_t *restrict src,
                              size_t n,
                              enum utf_error *stat);

#endif /* UTF_STRCONV_H */
