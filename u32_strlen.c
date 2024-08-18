#include "utf.h"

#define NULL_CHAR 0x00000000

size_t u32_strlen(const char32_t *s)
{
    size_t len = 0;
    while (*s++ != NULL_CHAR)
        len++;
    return len;
}
