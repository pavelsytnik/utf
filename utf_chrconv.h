/* The header is for internal usage only */

#ifndef utf_chrconv_h_
#define utf_chrconv_h_

#include "utf_char.h"

#include <stddef.h>

void utf_8_chr_to_32_(const utf_c8 *restrict src,
                      utf_c32 *restrict dst);
void utf_16_chr_to_32_(const utf_c16 *restrict src,
                       utf_c32 *restrict dst);
void utf_8_chr_to_16_(const utf_c8 *restrict src,
                      utf_c16 *restrict dst);
void utf_16_chr_to_8_(const utf_c16 *restrict src,
                      utf_c8 *restrict dst);
void utf_32_chr_to_8_(const utf_c32 *restrict src,
                      utf_c8 *restrict dst);
void utf_32_chr_to_16_(const utf_c32 *restrict src,
                       utf_c16 *restrict dst);

#endif /* !defined(utf_chrconv_h_) */
