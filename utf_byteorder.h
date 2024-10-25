#ifndef utf_byteorder_h_
#define utf_byteorder_h_

/* The x-postfix is temporary and only for the code to compile */
#if defined __BYTE_ORDER__
#    if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#        define UTF_BIG_ENDIANX
#    elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#        define UTF_LITTLE_ENDIANX
#    endif
#elif defined __BIG_ENDIAN__
#    define UTF_BIG_ENDIANX
#elif defined __LITTLE_ENDIAN__
#    define UTF_LITTLE_ENDIANX
#elif defined _WIN32
#    if defined _M_PPC
#        define UTF_BIG_ENDIANX
#    else
#        define UTF_LITTLE_ENDIANX
#    endif
#elif defined __ARMEB__ || defined __THUMBEB__ || defined __AARCH64EB__
#    define UTF_BIG_ENDIANX
#elif defined __ARMEL__ || defined __THUMBEL__ || defined __AARCH64EL__
#    define UTF_LITTLE_ENDIANX
#elif defined __MIPSEB__
#    define UTF_BIG_ENDIANX
#elif defined __MIPSEL__
#    define UTF_LITTLE_ENDIANX
#else
#    error "Unable to determine the system endianness"
#endif

#define utf_16_bswap(n)  \
 (                       \
     (n) >> 8 | (n) << 8 \
 )

#define utf_32_bswap(n)         \
 (                              \
      (n)               >> 24 | \
     ((n) & 0x00FF0000) >>  8 | \
     ((n) & 0x0000FF00) <<  8 | \
      (n)               << 24   \
 )

#if (defined __GNUC || defined __clang__) && defined __has_builtin
#    if __has_builtin(__builtin_bswap16) && __has_builtin(__builtin_bswap32)
#        undef utf_16_bswap
#        undef utf_32_bswap
#        define utf_16_bswap(n) __builtin_bswap16(n)
#        define utf_32_bswap(n) __builtin_bswap32(n)
#    endif
#elif defined _MSC_VER
#    include <stdlib.h>
#    undef utf_16_bswap
#    undef utf_32_bswap
#    define utf_16_bswap(n) _byteswap_ushort(n)
#    define utf_32_bswap(n) _byteswap_ulong(n)
#endif
#if defined UTF_BIG_ENDIANX
#    define utf_system_endianness() UTF_BIG_ENDIAN
#elif defined UTF_LITTLE_ENDIANX
#    define utf_system_endianness() UTF_LITTLE_ENDIAN
#endif

typedef enum utf_endianness utf_endianness;

enum utf_endianness {
    UTF_BIG_ENDIAN    = 1,
    UTF_LITTLE_ENDIAN = 2
};

#endif /* !defined(utf_byteorder_h_) */
