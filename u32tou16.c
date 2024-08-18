#include "utf.h"

// The ranges [D800; DFFF] and [110000; FFFFFFFF] aren't checked
utf16_t u32tou16(utf32_t c)
{
    if (c < 0x10000) {
        return c;
    } else {
        c -= 0x10000;
        return (c >> 10) + 0xD800 << 16 | (c & 0x3FF) + 0xDC00;
    }
}
