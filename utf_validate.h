#ifndef UTF_VALIDATE_H
#define UTF_VALIDATE_H

#include "utf_char.h"
#include "utf_error.h"

#include <stdint.h>

enum utf_error utf_u8next(const utf_c8 **strp, uint32_t *codepoint);
enum utf_error utf_u16next(const utf_c16 **strp, uint32_t *codepoint);

#endif /* UTF_VALIDATE_H */
