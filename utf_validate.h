#ifndef utf_validate_h_
#define utf_validate_h_

#include "utf_char.h"
#include "utf_error.h"

utf_error utf_8_next(const utf_c8 **iter);
utf_error utf_16_next(const utf_c16 **iter);
utf_error utf_32_next(const utf_c32 **iter);

#endif /* !defined(utf_validate_h_) */
