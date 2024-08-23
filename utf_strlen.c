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

size_t u8_strlen_s(const char8_t *s, size_t n)
{
    if (s == NULL)
        return 0;

    size_t len = 0;
    while (*s != NULLCHAR && n > 0) {
        if (!u8_iscontbyte(*s))
            len++;
        s++;
        n--;
    }

    return len;
}

size_t u16_strlen_s(const char16_t *s, size_t n)
{
    if (s == NULL)
        return 0;

    size_t len = 0;
    while (*s != NULLCHAR && n > 0) {
        if (u16_issurrogate(*s))
            s++;
        len++;
        s++;
        n--;
    }

    return len;
}

size_t u32_strlen_s(const char32_t *s, size_t n)
{
    if (s == NULL)
        return 0;

    size_t len = 0;
    while (*s != NULLCHAR && n > 0) {
        len++;
        s++;
        n--;
    }

    return len;
}
