#include "utf.h"

// Very simplified conversion
// Should be remade
utf8_t u32tou8(utf32_t c)
{
    if (c < 0x80) {
        return c;
    } else if (c < 0x800) {
        utf8_t out;
        out = ((c >> 6) | 0xC0) << 8;
        out |= c & 0x3F | 0x80;
        return out;
    } else if (c < 0x10000) {
        utf8_t out;
        out = ((c >> 12) | 0xE0) << 16;
        out |= ((c >> 6) & 0x3F | 0x80) << 8;
        out |= c & 0x3F | 0x80;
        return out;
    } else if (c < 0x110000) {
        utf8_t out;
        out = ((c >> 18) | 0xF0) << 24;
        out |= ((c >> 12) & 0x3F | 0x80) << 16;
        out |= ((c >> 6) & 0x3F | 0x80) << 8;
        out |= c & 0x3F | 0x80;
        return out;
    } else {
        return 0xFFFFFFFF;
    }
}
