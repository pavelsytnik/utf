#ifndef utf_strlen_h_
#define utf_strlen_h_

#include "utf_char.h"

#include <stddef.h>

size_t utf_8_strlen(const utf_c8 *s);
size_t utf_16_strlen(const utf_c16 *s);
size_t utf_32_strlen(const utf_c32 *s);

size_t utf_8_strlen_s(const utf_c8 *s, size_t n);
size_t utf_16_strlen_s(const utf_c16 *s, size_t n);
size_t utf_32_strlen_s(const utf_c32 *s, size_t n);

#endif /* !defined(utf_strlen_h_) */
