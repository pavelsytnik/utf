#include "utf_validate.h"

utf_error utf_8_next(const utf_c8 **iter)
{
    int len;
    const utf_c8 *orig = *iter;

    if ((len = utf_8_length_from_lead(**iter)) == 0)
        return UTF_INVALID_LEAD;

    (*iter)++;

    if (len == 1)
        return UTF_OK;

    while (--len > 0) {
        if (!utf_8_is_trail(**iter))
            return UTF_INVALID_TRAIL;
        (*iter)++;
    }

    if (utf_8_is_invalid_code_point(orig))
        return UTF_INVALID_CODE_POINT;
    if (utf_8_is_overlong_sequence(orig))
        return UTF_OVERLONG_SEQUENCE;

    return UTF_OK;
}

utf_error utf_16_next(const utf_c16 **iter)
{
    if (utf_is_trail_surrogate(**iter))
        return UTF_INVALID_LEAD;

    if (utf_is_lead_surrogate(**iter)) {
        (*iter)++;
        if (!utf_is_trail_surrogate(**iter))
            return UTF_INVALID_TRAIL;
    }

    (*iter)++;
    return UTF_OK;
}

utf_error utf_32_next(const utf_c32 **iter)
{
    if (!utf_is_valid_code_point(**iter))
        return UTF_INVALID_CODE_POINT;

    (*iter)++;
    return UTF_OK;
}
