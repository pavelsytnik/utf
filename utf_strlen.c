#include "utf.h"

#define NULLCHAR 0

size_t utf_str8len(const char8_t *s)
{
    size_t len = 0;
    while (*s++ != NULLCHAR)
        if (!UTF_IS_TRAIL(*s))
            len++;
    return len;
}

size_t utf_str16len(const char16_t *s)
{
    size_t len = 0;
    while (*s++ != NULLCHAR) {
        len++;
        if (UTF_IS_SURROGATE(*s))
            s++;
    }
    return len;
}

size_t utf_str32len(const char32_t *s)
{
    size_t len = 0;
    while (*s++ != NULLCHAR)
        len++;
    return len;
}

size_t utf_str8len_s(const char8_t *s, size_t n)
{
    if (s == NULL)
        return 0;

    size_t len = 0;
    while (*s != NULLCHAR && n > 0) {
        if (!UTF_IS_TRAIL(*s))
            len++;
        s++;
        n--;
    }

    return len;
}

size_t utf_str16len_s(const char16_t *s, size_t n)
{
    if (s == NULL)
        return 0;

    size_t len = 0;
    while (*s != NULLCHAR && n > 0) {
        if (UTF_IS_SURROGATE(*s))
            s++;
        len++;
        s++;
        n--;
    }

    return len;
}

size_t utf_str32len_s(const char32_t *s, size_t n)
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
