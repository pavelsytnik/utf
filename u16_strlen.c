#include "utf.h"

#define NULL_CHAR 0x0000

size_t u16_strlen(const char16_t *s)
{
    size_t len = 0;
    while (*s++ != NULL_CHAR) {
        len++;
        if (u16_issurrogate(*s))
            s++;
    }
    return len;
}
