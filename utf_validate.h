#ifndef utf_validate_h_
#define utf_validate_h_

#include "utf_char.h"
#include "utf_error.h"

utf_error utf_8_next(const utf_c8 **strp, uint32_t *codepoint);
utf_error utf_16_next(const utf_c16 **strp, uint32_t *codepoint);

#endif /* !defined(utf_validate_h_) */
