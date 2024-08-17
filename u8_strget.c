#include "utf.h"

size_t u8_strget(const char *s, size_t i)
{
    size_t shift = 0;
    while (i-- > 0) {
        if ((s[shift] & 0x80) == 0x00) {
            shift += 1;
        } else if ((s[shift] & 0xE0) == 0xC0) {
            shift += 2;
        } else if ((s[shift] & 0xF0) == 0xE0) {
            shift += 3;
        } else if ((s[shift] & 0xF8) == 0xF0) {
            shift += 4;
        }
    }
    return shift;
}
