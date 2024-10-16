#ifndef utf_strconv_h_
#define utf_strconv_h_

#include "utf_char.h"
#include "utf_error.h"

#include <stddef.h>

void utf_8_to_16(utf_c16 *restrict dst, const utf_c8 *restrict src);
void utf_16_to_8(utf_c8 *restrict dst, const utf_c16 *restrict src);
void utf_8_to_32(utf_c32 *restrict dst, const utf_c8 *restrict src);
void utf_32_to_8(utf_c8 *restrict dst, const utf_c32 *restrict src);
void utf_32_to_16(utf_c16 *restrict dst, const utf_c32 *restrict src);
void utf_16_to_32(utf_c32 *restrict dst, const utf_c16 *restrict src);

const utf_c8 *utf_8_to_32_s(utf_c32 *restrict dst,
                            const utf_c8 *restrict src,
                            size_t n,
                            utf_error *stat);
const utf_c16 *utf_16_to_32_s(utf_c32 *restrict dst,
                              const utf_c16 *restrict src,
                              size_t n,
                              utf_error *stat);

#endif /* !defined(utf_strconv_h_) */
