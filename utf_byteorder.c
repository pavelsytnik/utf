#include "utf_byteorder.h"

utf_c16 utf_16_byteswap(utf_c16 n)
{
    return n << 8 | n >> 8;
}

utf_c32 utf_32_byteswap(utf_c32 n)
{
    n = n << 8 & 0xFF00FF00 | n >> 8 & 0x00FF00FF;
    return n << 16 | n >> 16;
}

utf_endianness utf_system_endianness(void)
{
    /* must be rewritten as soon as possible */
    utf_c16 word = 0x0001;
    utf_c8 octet = *(utf_c8 *) &word;

    return octet ? UTF_LITTLE_ENDIAN : UTF_BIG_ENDIAN;
}
