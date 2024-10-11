#ifndef UTF_CHAR_H
#define UTF_CHAR_H

#include <stdint.h>

#define UTF_CODEPOINT_MAX 0x10FFFFu

#define UTF_LEAD_SURROGATE_MIN  0xD800u
#define UTF_LEAD_SURROGATE_MAX  0xDBFFu
#define UTF_TRAIL_SURROGATE_MIN 0xDC00u
#define UTF_TRAIL_SURROGATE_MAX 0xDFFFu

#define utf_is_c8trail(a) \
    (((a) & 0xC0) == 0x80)

#define utf_is_lead_surrogate(a) \
    ((a) >= UTF_LEAD_SURROGATE_MIN && (a) <= UTF_LEAD_SURROGATE_MAX)

#define utf_is_trail_surrogate(a) \
    ((a) >= UTF_TRAIL_SURROGATE_MIN && (a) <= UTF_TRAIL_SURROGATE_MAX)

#define utf_is_surrogate(a) \
    ((a) >= UTF_LEAD_SURROGATE_MIN && (a) <= UTF_TRAIL_SURROGATE_MAX)

#define utf_is_valid_codepoint(a) \
    ((a) <= UTF_CODEPOINT_MAX && !utf_is_surrogate(a))

#define utf_s8sz(i)  ((i) * 4 + 1)
#define utf_s16sz(i) ((i) * 4 + 2)
#define utf_s32sz(i) ((i) * 4 + 4)

#define utf_s8arrsz(i)  ((i) * 4 + 1)
#define utf_s16arrsz(i) ((i) * 2 + 1)
#define utf_s32arrsz(i) ((i) + 1)

typedef unsigned char utf_c8;
typedef uint_least16_t utf_c16;
typedef uint_least32_t utf_c32;

#endif /* UTF_CHAR_H */
