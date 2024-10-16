#ifndef utf_strget_h_
#define utf_strget_h_

#include "utf_char.h"

#include <stddef.h>

utf_c32 utf_8_strget(const utf_c8 *s, size_t i);

utf_c8 *utf_8_strat(const utf_c8 *s, size_t i);

#endif /* !defined(utf_strget_h_) */
