#include "utf_strlen.h"

#define UTF_NULL_ 0

size_t utf_8_strlen(const utf_c8 *s)
{
    size_t len = 0;
    while (*s++ != UTF_NULL_)
        if (!utf_8_is_trail(*s))
            len++;
    return len;
}

size_t utf_16_strlen(const utf_c16 *s)
{
    size_t len = 0;
    while (*s++ != UTF_NULL_) {
        len++;
        if (utf_is_surrogate(*s))
            s++;
    }
    return len;
}

size_t utf_32_strlen(const utf_c32 *s)
{
    size_t len = 0;
    while (*s++ != UTF_NULL_)
        len++;
    return len;
}

size_t utf_8_strlen_s(const utf_c8 *s, size_t n)
{
    if (s == NULL)
        return 0;

    size_t len = 0;
    while (*s != UTF_NULL_ && n > 0) {
        if (!utf_8_is_trail(*s))
            len++;
        s++;
        n--;
    }

    return len;
}

size_t utf_16_strlen_s(const utf_c16 *s, size_t n)
{
    if (s == NULL)
        return 0;

    size_t len = 0;
    while (*s != UTF_NULL_ && n > 0) {
        if (utf_is_surrogate(*s))
            s++;
        len++;
        s++;
        n--;
    }

    return len;
}

size_t utf_32_strlen_s(const utf_c32 *s, size_t n)
{
    if (s == NULL)
        return 0;

    size_t len = 0;
    while (*s != UTF_NULL_ && n > 0) {
        len++;
        s++;
        n--;
    }

    return len;
}
