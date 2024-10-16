#ifndef utf_byteorder_h_
#define utf_byteorder_h_

#include "utf_char.h"

typedef enum utf_endianness utf_endianness;

enum utf_endianness {
    UTF_BIG_ENDIAN    = 1,
    UTF_LITTLE_ENDIAN = 2
};

utf_endianness utf_system_endianness(void);

utf_c16 utf_16_byteswap(utf_c16 n);
utf_c32 utf_32_byteswap(utf_c32 n);

#endif /* !defined(utf_byteorder_h_) */
