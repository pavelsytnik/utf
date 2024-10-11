#ifndef UTF_STRLEN_H
#define UTF_STRLEN_H

#include "utf_char.h"

#include <stddef.h>

size_t utf_s8len(const utf_c8 *s);
size_t utf_s16len(const utf_c16 *s);
size_t utf_s32len(const utf_c32 *s);

size_t utf_s8len_s(const utf_c8 *s, size_t n);
size_t utf_s16len_s(const utf_c16 *s, size_t n);
size_t utf_s32len_s(const utf_c32 *s, size_t n);

#endif /* UTF_STRLEN_H */
