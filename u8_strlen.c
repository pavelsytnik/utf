#include <stdlib.h>

size_t u8_strlen(const char *s)
{
    size_t len = 0;
    while (*s++ != '\0')
        if ((*s & 0xC0) != 0x80)
            len++;
    return len;
}
