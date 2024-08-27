#ifndef UTF_BYTEORDER_H
#define UTF_BYTEORDER_H

#include <stdint.h>

enum utf_endianness {
    UTF_LITTLE_ENDIAN = 1,
    UTF_BIG_ENDIAN = 2
};

enum utf_endianness utf_receive_endianness(void);

uint16_t utf_swapbytes_uint16(uint16_t n);
uint32_t utf_swapbytes_uint32(uint32_t n);

#endif /* UTF_BYTEORDER_H */
