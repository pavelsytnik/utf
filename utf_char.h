#ifndef UTF_CHAR_H
#define UTF_CHAR_H

#include <stdint.h>

#define UTF_CODEPOINT_MAX 0x10FFFFu

#define UTF_LEAD_SURROGATE_MIN 0xD800u
#define UTF_LEAD_SURROGATE_MAX 0xDBFFu
#define UTF_TRAIL_SURROGATE_MIN 0xDC00u
#define UTF_TRAIL_SURROGATE_MAX 0xDFFFu

#define UTF_IS_TRAIL(a) \
    (((a) & 0xC0) == 0x80)

#define UTF_IS_LEAD_SURROGATE(a) \
    ((a) >= UTF_LEAD_SURROGATE_MIN && (a) <= UTF_LEAD_SURROGATE_MAX)

#define UTF_IS_TRAIL_SURROGATE(a) \
    ((a) >= UTF_TRAIL_SURROGATE_MIN && (a) <= UTF_TRAIL_SURROGATE_MAX)

#define UTF_IS_SURROGATE(a) \
    ((a) >= UTF_LEAD_SURROGATE_MIN && (a) <= UTF_TRAIL_SURROGATE_MAX)

#define UTF_IS_VALID_CODEPOINT(a) \
    ((a) <= UTF_CODEPOINT_MAX && !UTF_IS_SURROGATE(a))

/* Compatible with the corresponding
 * type definitions of <uchar.h>
 */
typedef unsigned char char8_t;
typedef uint_least16_t char16_t;
typedef uint_least32_t char32_t;

#endif /* UTF_CHAR_H */
