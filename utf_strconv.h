#ifndef utf_strconv_h_
#define utf_strconv_h_

#include "utf_char.h"
#include "utf_error.h"

#include <stddef.h>

void utf_8_to_16(const utf_c8 *restrict src, utf_c16 *restrict dst);
void utf_16_to_8(const utf_c16 *restrict src, utf_c8 *restrict dst);
void utf_8_to_32(const utf_c8 *restrict src, utf_c32 *restrict dst);
void utf_32_to_8(const utf_c32 *restrict src, utf_c8 *restrict dst);
void utf_32_to_16(const utf_c32 *restrict src, utf_c16 *restrict dst);
void utf_16_to_32(const utf_c16 *restrict src, utf_c32 *restrict dst);

const utf_c8 *utf_8_to_32_s(const utf_c8 *restrict src,
                            utf_c32 *restrict dst,
                            size_t n,
                            utf_error *stat);
const utf_c16 *utf_16_to_32_s(const utf_c16 *restrict src,
                              utf_c32 *restrict dst,
                              size_t n,
                              utf_error *stat);

#endif /* !defined(utf_strconv_h_) */
