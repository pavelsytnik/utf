#ifndef UTF_VALIDATE_H
#define UTF_VALIDATE_H

#include "utf_char.h"
#include "utf_error.h"

#include <stdint.h>

enum utf_error utf_u8next(const char8_t **strp, uint32_t *codepoint);
enum utf_error utf_u16next(const char16_t **strp, uint32_t *codepoint);

#endif /* UTF_VALIDATE_H */
