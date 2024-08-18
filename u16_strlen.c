#include "utf.h"

#define NULL_CHAR 0x0000

size_t u16_strlen(const uint16_t *s)
{
    size_t len = 0;
    while (*s++ != NULL_CHAR) {
        len++;
        if (*s >= 0xD800 && *s <= 0xDFFF)
            s++;
    }
    return len;
}
