#ifndef UTF_STRCONV_H
#define UTF_STRCONV_H

#include "utf_char.h"
#include "utf_error.h"

#include <stddef.h>

void utf_s8to16(utf_c16 *restrict dst, const utf_c8 *restrict src);
void utf_s16to8(utf_c8 *restrict dst, const utf_c16 *restrict src);
void utf_s8to32(utf_c32 *restrict dst, const utf_c8 *restrict src);
void utf_s32to8(utf_c8 *restrict dst, const utf_c32 *restrict src);
void utf_s32to16(utf_c16 *restrict dst, const utf_c32 *restrict src);
void utf_s16to32(utf_c32 *restrict dst, const utf_c16 *restrict src);

const utf_c8 *utf_s8to32_s(utf_c32 *restrict dst,
                           const utf_c8 *restrict src,
                           size_t n,
                           enum utf_error *stat);

#endif /* UTF_STRCONV_H */
