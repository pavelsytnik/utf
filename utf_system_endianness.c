#include "utf.h"

enum utf_endianness utf_receive_endianness(void)
{
    uint16_t word = 0x0001;
    uint8_t octet = *(uint8_t *) &word;

    return octet ? UTF_LITTLE_ENDIAN : UTF_BIG_ENDIAN;
}
