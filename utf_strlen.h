#ifndef UTF_STRLEN_H
#define UTF_STRLEN_H

#include "utf_char.h"

#include <stddef.h>

size_t utf_str8len(const char8_t *s);
size_t utf_str16len(const char16_t *s);
size_t utf_str32len(const char32_t *s);

size_t utf_str8len_s(const char8_t *s, size_t n);
size_t utf_str16len_s(const char16_t *s, size_t n);
size_t utf_str32len_s(const char32_t *s, size_t n);

#endif /* UTF_STRLEN_H */
