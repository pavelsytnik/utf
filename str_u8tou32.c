#include "utf.h"

// Don't know why I'm writing this again... TOTALLY UNSAFE!!!
void str_u8tou32(char32_t *restrict dst, const char8_t *restrict src)
{
    size_t src_len = u8_strlen(src);
    for (size_t i = 0; i < src_len; i++)
    {
        dst[i] = u8_strget(src, i);
    }
    dst[src_len] = 0;
}
