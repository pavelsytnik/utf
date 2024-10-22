#include "utf_strconv.h"

#include "utf_chrconv.h"

void utf_8_to_16(const utf_c8 *restrict src, utf_c16 *restrict dst)
{
    while (*src != 0) {
        utf_8_chr_to_16_(src, dst);

        src += utf_8_length_from_lead(*src);
        dst += !utf_is_surrogate(*dst) ? 1 : 2;
    }

    *dst = 0;
}

void utf_8_to_32(const utf_c8 *restrict src, utf_c32 *restrict dst)
{
    while (*src != 0) {
        utf_8_chr_to_32_(src, dst);

        src += utf_8_length_from_lead(*src);
        dst++;
    }

    *dst = 0;
}

void utf_16_to_8(const utf_c16 *restrict src, utf_c8 *restrict dst)
{
    while (*src != 0) {
        utf_16_chr_to_8_(src, dst);

        src += !utf_is_surrogate(*src) ? 1 : 2;
        dst += utf_8_length_from_lead(*dst);
    }

    *dst = 0;
}

void utf_16_to_32(const utf_c16 *restrict src, utf_c32 *restrict dst)
{
    while (*src != 0) {
        utf_16_chr_to_32_(src, dst);

        src += !utf_is_surrogate(*src) ? 1 : 2;
        dst++;
    }

    *dst = 0;
}

void utf_32_to_8(const utf_c32 *restrict src, utf_c8 *restrict dst)
{
    while (*src != 0) {
        utf_32_chr_to_8_(src, dst);

        src++;
        dst += utf_8_length_from_lead(*dst);
    }

    *dst = 0;
}

void utf_32_to_16(const utf_c32 *restrict src, utf_c16 *restrict dst)
{
    while (*src != 0) {
        utf_32_chr_to_16_(src, dst);

        src++;
        dst += !utf_is_surrogate(*dst) ? 1 : 2;
    }

    *dst = 0;
}
