// For internal usage
// Adapted Nemanja Trifunovic's code
// https://github.com/nemtrif/utfcpp

#include "utf.h"

#define UTF_NEXT_TRAIL_OR_FAIL(str) do { \
    if (*++(str) == 0) \
        return UTF_NOT_ENOUGH_ROOM; \
    if ((*(str) & 0xC0) != 0x80) \
        return UTF_INVALID_TRAIL; \
} while (0)

#define UTF_IS_OVERLONG_SEQUENCE(codepoint, length) \
    ((codepoint) <= 0x7F   && (length) > 1 || \
     (codepoint) <= 0x7FF  && (length) > 2 || \
     (codepoint) <= 0xFFFF && (length) > 3 )

#define UTF_SEQUENCE_LENGTH(c) \
    (((c) & 0x80) == 0x00 ? 1 : \
     ((c) & 0xE0) == 0xC0 ? 2 : \
     ((c) & 0xF0) == 0xE0 ? 3 : \
     ((c) & 0xF8) == 0xF0 ? 4 : \
                            0  )

static enum utf_error utf_decode_sequence(const utf_c8 **strp,
                                          uint32_t *codepoint,
                                          int length)
{
    switch (length) {
    case 1:
        *codepoint = **strp;

        break;
    case 2:
        *codepoint = (**strp & 0x1F) << 6;

        UTF_NEXT_TRAIL_OR_FAIL(*strp);
        *codepoint |= **strp & 0x3F;

        break;
    case 3:
        *codepoint = (**strp & 0x0F) << 12;

        UTF_NEXT_TRAIL_OR_FAIL(*strp);
        *codepoint |= (**strp & 0x3F) << 6;

        UTF_NEXT_TRAIL_OR_FAIL(*strp);
        *codepoint |= **strp & 0x3F;

        break;
    case 4:
        *codepoint = (**strp & 0x07) << 18;

        UTF_NEXT_TRAIL_OR_FAIL(*strp);
        *codepoint |= (**strp & 0x3F) << 12;

        UTF_NEXT_TRAIL_OR_FAIL(*strp);
        *codepoint |= (**strp & 0x3F) << 6;

        UTF_NEXT_TRAIL_OR_FAIL(*strp);
        *codepoint |= **strp & 0x3F;

        break;
    default:
        return UTF_INVALID_LEAD;
    }

    return UTF_OK;
}

enum utf_error utf_u8next(const utf_c8 **strp, uint32_t *codepoint)
{
    if (**strp == 0)
        return UTF_NOT_ENOUGH_ROOM;

    uint32_t cp = 0;
    int len = UTF_SEQUENCE_LENGTH(**strp);
    enum utf_error err = utf_decode_sequence(strp, &cp, len);

    if (err != UTF_OK)
        return err;

    if (UTF_IS_OVERLONG_SEQUENCE(cp, len))
        err = UTF_OVERLONG_SEQUENCE;
    else if (!utf_is_valid_codepoint(cp))
        err = UTF_INVALID_CODEPOINT;

    if (err != UTF_OK) {
        *strp -= len - 1;
        return err;
    }

    *codepoint = cp;
    ++*strp;

    return UTF_OK;
}

enum utf_error utf_u16next(const utf_c16 **strp, uint32_t *codepoint)
{
    if (**strp == 0)
        return UTF_NOT_ENOUGH_ROOM;

    uint32_t cp = 0;

    if (!utf_is_surrogate(**strp)) {
        cp = **strp;
    } else if (utf_is_lead_surrogate(**strp)) {
        cp = **strp - UTF_LEAD_SURROGATE_MIN << 10;

        if (*++*strp == 0)
            return UTF_NOT_ENOUGH_ROOM;
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
