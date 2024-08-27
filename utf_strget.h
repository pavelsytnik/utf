#ifndef UTF_STRGET_H
#define UTF_STRGET_H

#include "utf_char.h"

#include <stddef.h>

char32_t utf_str8get(const char8_t *s, size_t i);

char8_t *utf_str8at(const char8_t *s, size_t i);

#endif /* UTF_STRGET_H */
