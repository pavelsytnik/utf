#include "utf.h"

// Unsafe code
// Doesn't check whether the code point is valid
utf32_t u8tou32(utf8_t c)
{
    if ((c & 0x80) == 0x00) {
        return c;
    } else if ((c >> 8 & 0xE0) == 0xC0) {
        utf32_t out = 0;
        out |= (c >> 8 & 0x1F) << 6;
        out |= c & 0x3F;
        return out;
    } else if ((c >> 16 & 0xF0) == 0xE0) {
        utf32_t out = 0;
        out |= (c >> 16 & 0x0F) << 12;
        out |= (c >> 8  & 0x3F) << 6;
        out |= c & 0x3F;
        return out;
    } else if ((c >> 24 & 0xF8) == 0xF0) {
        utf32_t out = 0;
        out |= (c >> 24 & 0x07) << 18;
        out |= (c >> 16 & 0x3F) << 12;
        out |= (c >> 8  & 0x3F) << 6;
        out |= c & 0x3F;
        return out;
    } else {
        return 0xFFFFFFFF;
    }
}
