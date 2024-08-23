// For internal usage
// Adapted Nemanja Trifunovic's code
// https://github.com/nemtrif/utfcpp

#include "utf.h"

#define UTF8_TRY_INCREMENT_STRING_POINTER(strp) do { \
    if (++*(strp) == 0) \
        return UTF_NOT_ENOUGH_ROOM; \
    if ((**(strp) & 0xC0) != 0x80) \
        return UTF_INVALID_TRAIL; \
} while (0)

static int utf8_is_overlong(uint32_t codepoint, int length)
{
    return codepoint <= 0x7F   && length > 1 ||
           codepoint <= 0x7FF  && length > 2 ||
           codepoint <= 0xFFFF && length > 3  ;
}

static int utf8_seqlen(const char8_t *c)
{
    if ((*c & 0x80) == 0x00)
        return 1;
    else if ((*c & 0xE0) == 0xC0)
        return 2;
    else if ((*c & 0xF0) == 0xE0)
        return 3;
    else if ((*c & 0xF8) == 0xF0)
        return 4;
    else
        return 0;
}

static enum utf_error utf8_getseq(const char8_t **p_s,
                                  uint32_t *p_codepoint,
                                  int length)
{
    switch (length) {
    case 1:
        *p_codepoint = **p_s;

        break;
    case 2:
        *p_codepoint = (**p_s & 0x1F) << 6;

        UTF8_TRY_INCREMENT_STRING_POINTER(p_s);
        *p_codepoint |= **p_s & 0x3F;

        break;
    case 3:
        *p_codepoint = (**p_s & 0x0F) << 12;

        UTF8_TRY_INCREMENT_STRING_POINTER(p_s);
        *p_codepoint |= (**p_s & 0x3F) << 6;

        UTF8_TRY_INCREMENT_STRING_POINTER(p_s);
        *p_codepoint |= **p_s & 0x3F;

        break;
    case 4:
        *p_codepoint = (**p_s & 0x07) << 18;

        UTF8_TRY_INCREMENT_STRING_POINTER(p_s);
        *p_codepoint |= (**p_s & 0x3F) << 12;

        UTF8_TRY_INCREMENT_STRING_POINTER(p_s);
        *p_codepoint |= (**p_s & 0x3F) << 6;

        UTF8_TRY_INCREMENT_STRING_POINTER(p_s);
        *p_codepoint |= **p_s & 0x3F;

        break;
    default:
        return UTF_INVALID_LEAD;
    }

    return UTF_OK;
}

enum utf_error utf8_validate_next(const char8_t **p_s, uint32_t *p_codepoint)
{
    if (**p_s == 0)
        return UTF_NOT_ENOUGH_ROOM;

    uint32_t cp = 0;
    int len = utf8_seqlen(*p_s);
    enum utf_error err = utf8_getseq(p_s, &cp, len);

    if (err != UTF_OK)
        return err;

    if (utf8_is_overlong(cp, len))
        return UTF_OVERLONG_SEQUENCE;

    if (!UTF_IS_VALID_CODEPOINT(cp))
        return UTF_INVALID_CODEPOINT;

    *p_codepoint = cp;
    ++*p_s;

    return UTF_OK;
}
