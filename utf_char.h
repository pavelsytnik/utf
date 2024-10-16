#ifndef utf_char_h_
#define utf_char_h_

#include <stdint.h>

#define UTF_CODE_POINT_MAX 0x10FFFFu

#define UTF_LEAD_SURROGATE_MIN  0xD800u
#define UTF_LEAD_SURROGATE_MAX  0xDBFFu
#define UTF_TRAIL_SURROGATE_MIN 0xDC00u
#define UTF_TRAIL_SURROGATE_MAX 0xDFFFu

#define utf_8_is_overlong_sequence(c)   \
    ((c)[0] <  0xC2                  || \
     (c)[0] == 0xE0 && (c)[1] < 0xA0 || \
     (c)[0] == 0xF0 && (c)[1] < 0x90)

#define utf_8_is_invalid_code_point(c)  \
    ((c)[0] == 0xED && (c)[1] > 0x9F || \
     (c)[0] == 0xF4 && (c)[1] > 0x8F || \
     (c)[0] >  0xF4)

#define utf_8_is_lead_1(a) \
    (((a) & 0x80) == 0x00)

#define utf_8_is_lead_2(a) \
    (((a) & 0xE0) == 0xC0)

#define utf_8_is_lead_3(a) \
    (((a) & 0xF0) == 0xE0)

#define utf_8_is_lead_4(a) \
    (((a) & 0xF8) == 0xF0)

#define utf_8_is_trail(a) \
    (((a) & 0xC0) == 0x80)

#define utf_8_length_from_lead(a) \
    (utf_8_is_lead_1(a) ? 1 :     \
     utf_8_is_lead_2(a) ? 2 :     \
     utf_8_is_lead_3(a) ? 3 :     \
     utf_8_is_lead_4(a) ? 4 :     \
                          0)

#define utf_is_lead_surrogate(c) \
    ((c) >= UTF_LEAD_SURROGATE_MIN && (c) <= UTF_LEAD_SURROGATE_MAX)

#define utf_is_trail_surrogate(c) \
    ((c) >= UTF_TRAIL_SURROGATE_MIN && (c) <= UTF_TRAIL_SURROGATE_MAX)

#define utf_is_surrogate(c) \
    ((c) >= UTF_LEAD_SURROGATE_MIN && (c) <= UTF_TRAIL_SURROGATE_MAX)

#define utf_is_valid_code_point(c) \
    ((c) <= UTF_CODE_POINT_MAX && !utf_is_surrogate(c))

#define utf_8_sz(i)  ((i) * 4 + 1)
#define utf_16_sz(i) ((i) * 4 + 2)
#define utf_32_sz(i) ((i) * 4 + 4)

#define utf_8_n(i)  ((i) * 4 + 1)
#define utf_16_n(i) ((i) * 2 + 1)
#define utf_32_n(i) ((i) + 1)

typedef unsigned char utf_c8;
typedef uint_least16_t utf_c16;
typedef uint_least32_t utf_c32;

#endif /* !defined(utf_char_h_) */
