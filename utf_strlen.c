#include "utf.h"

#define NULLCHAR 0

size_t utf_s8len(const utf_c8 *s)
{
    size_t len = 0;
    while (*s++ != NULLCHAR)
        if (!utf_is_c8trail(*s))
            len++;
    return len;
}

size_t utf_s16len(const utf_c16 *s)
{
    size_t len = 0;
    while (*s++ != NULLCHAR) {
        len++;
        if (utf_is_surrogate(*s))
            s++;
    }
    return len;
}

size_t utf_s32len(const utf_c32 *s)
{
    size_t len = 0;
    while (*s++ != NULLCHAR)
        len++;
    return len;
}

size_t utf_s8len_s(const utf_c8 *s, size_t n)
{
    if (s == NULL)
        return 0;

    size_t len = 0;
    while (*s != NULLCHAR && n > 0) {
        if (!utf_is_c8trail(*s))
            len++;
        s++;
        n--;
    }

    return len;
}

size_t utf_s16len_s(const utf_c16 *s, size_t n)
{
    if (s == NULL)
        return 0;

    size_t len = 0;
    while (*s != NULLCHAR && n > 0) {
        if (utf_is_surrogate(*s))
            s++;
        len++;
        s++;
        n--;
    }

    return len;
}

size_t utf_s32len_s(const utf_c32 *s, size_t n)
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
