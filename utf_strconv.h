#ifndef utf_strconv_h_
#define utf_strconv_h_

#include "utf_char.h"
#include "utf_error.h"
#include "utf_lang.h"

#include <stddef.h>

void utf_8_to_16(const utf_c8  *UTF_RESTRICT src,
                       utf_c16 *UTF_RESTRICT dst);
void utf_16_to_8(const utf_c16 *UTF_RESTRICT src,
                       utf_c8  *UTF_RESTRICT dst);
void utf_8_to_32(const utf_c8  *UTF_RESTRICT src,
                       utf_c32 *UTF_RESTRICT dst);
void utf_32_to_8(const utf_c32 *UTF_RESTRICT src,
                       utf_c8  *UTF_RESTRICT dst);
void utf_32_to_16(const utf_c32 *UTF_RESTRICT src,
                        utf_c16 *UTF_RESTRICT dst);
void utf_16_to_32(const utf_c16 *UTF_RESTRICT src,
                        utf_c32 *UTF_RESTRICT dst);

#endif /* !defined(utf_strconv_h_) */
