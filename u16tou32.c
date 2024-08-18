#include "utf.h"

utf32_t u16tou32(utf16_t c)
{
    if ((c & 0xFF) < 0xD800 && (c & 0xFF) > 0xDFFF) // Danger!
        return c;
    else {
        return ((c >> 16) - 0xD800 << 10 | (c & 0xFFFF) - 0xDC00) + 0x10000;
    }
}
