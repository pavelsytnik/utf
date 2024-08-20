#include "utf.h"

#define NULLCHAR 0

size_t u8_strlen(const char8_t *s)
{
    size_t len = 0;
    while (*s++ != NULLCHAR)
        if (!u8_iscontbyte(*s))
            len++;
    return len;
}

size_t u16_strlen(const char16_t *s)
{
    size_t len = 0;
    while (*s++ != NULLCHAR) {
        len++;
        if (u16_issurrogate(*s))
            s++;
    }
    return len;
}

size_t u32_strlen(const char32_t *s)
{
    size_t len = 0;
    while (*s++ != NULLCHAR)
        len++;
    return len;
}
