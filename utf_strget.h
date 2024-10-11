#ifndef UTF_STRGET_H
#define UTF_STRGET_H

#include "utf_char.h"

#include <stddef.h>

utf_c32 utf_s8get(const utf_c8 *s, size_t i);

utf_c8 *utf_s8at(const utf_c8 *s, size_t i);

#endif /* UTF_STRGET_H */
