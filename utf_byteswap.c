#include "utf.h"

uint16_t utf_swapbytes_uint16(uint16_t n)
{
    return n << 8 | n >> 8;
}

uint32_t utf_swapbytes_uint32(uint32_t n)
{
    n = n << 8 & 0xFF00FF00 | n >> 8 & 0x00FF00FF;
    return n << 16 | n >> 16;
}
