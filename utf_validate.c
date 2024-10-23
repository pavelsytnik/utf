#include "utf_validate.h"

#define UTF_NEXT_TRAIL_OR_FAIL_(str) \
    if (*++(str) == 0)               \
        return UTF_TRUNCATED;        \
    if ((*(str) & 0xC0) != 0x80)     \
        return UTF_INVALID_TRAIL;    \

#define utf_8_is_overlong_sequence_(codepoint, length) \
    ((codepoint) <= 0x7F   && (length) > 1 ||          \
     (codepoint) <= 0x7FF  && (length) > 2 ||          \
     (codepoint) <= 0xFFFF && (length) > 3 )

static utf_error utf_8_decode_(const utf_c8 **strp,
                               utf_c32 *codepoint,
                               int length)
{
    switch (length) {
    case 1:
        *codepoint = **strp;

        break;
    case 2:
        *codepoint = (**strp & 0x1F) << 6;

        UTF_NEXT_TRAIL_OR_FAIL_(*strp)
        *codepoint |= **strp & 0x3F;

        break;
    case 3:
        *codepoint = (**strp & 0x0F) << 12;

        UTF_NEXT_TRAIL_OR_FAIL_(*strp)
        *codepoint |= (**strp & 0x3F) << 6;

        UTF_NEXT_TRAIL_OR_FAIL_(*strp)
        *codepoint |= **strp & 0x3F;

        break;
    case 4:
        *codepoint = (**strp & 0x07) << 18;

        UTF_NEXT_TRAIL_OR_FAIL_(*strp)
        *codepoint |= (**strp & 0x3F) << 12;

        UTF_NEXT_TRAIL_OR_FAIL_(*strp)
        *codepoint |= (**strp & 0x3F) << 6;

        UTF_NEXT_TRAIL_OR_FAIL_(*strp)
        *codepoint |= **strp & 0x3F;

        break;
    default:
        return UTF_INVALID_LEAD;
    }

    return UTF_OK;
}

utf_error utf_8_next(const utf_c8 **strp, utf_c32 *codepoint)
{
    if (**strp == 0)
        return UTF_TRUNCATED;

    utf_c32 cp = 0;
    int len = utf_8_length_from_lead(**strp);
    utf_error err = utf_8_decode_(strp, &cp, len);

    if (err != UTF_OK)
        return err;

    if (utf_8_is_overlong_sequence_(cp, len))
        err = UTF_OVERLONG_SEQUENCE;
    else if (!utf_is_valid_code_point(cp))
        err = UTF_INVALID_CODE_POINT;

    if (err != UTF_OK) {
        *strp -= len - 1;
        return err;
    }

    *codepoint = cp;
    ++*strp;

    return UTF_OK;
}

utf_error utf_16_next(const utf_c16 **strp, utf_c32 *codepoint)
{
    if (**strp == 0)
        return UTF_TRUNCATED;

    utf_c32 cp = 0;

    if (!utf_is_surrogate(**strp)) {
        cp = **strp;
    } else if (utf_is_lead_surrogate(**strp)) {
        cp = **strp - UTF_LEAD_SURROGATE_MIN << 10;

        if (*++*strp == 0)
            return UTF_TRUNCATED;
        if (!utf_is_trail_surrogate(**strp))
            return UTF_INVALID_TRAIL;

        cp |= **strp - UTF_TRAIL_SURROGATE_MIN;
        cp += 0x10000;
    } else {
        return UTF_INVALID_LEAD;
    }

    *codepoint = cp;
    ++*strp;

    return UTF_OK;
}
