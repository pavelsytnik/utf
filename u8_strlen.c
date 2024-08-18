#include <stdlib.h>
#include "utf.h"

size_t u8_strlen(const char *s)
{
    size_t len = 0;
    while (*s++ != '\0')
        if (!u8_iscontbyte(*s))
            len++;
    return len;
}
