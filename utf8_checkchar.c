// For internal usage
// Adapted Nemanja Trifunovic's code
// https://github.com/nemtrif/utfcpp

#include "utf.h"

#define UTF8_TRY_INCREMENT_STRING_POINTER(str) do {\
    enum utf_error err = utf_inc_strp(str);\
    if (err) return err;\
} while (0)

static int utf8_is_overlong(uint32_t codepoint, int length)
{
    return codepoint <= 0x7F   && length > 1 ||
           codepoint <= 0x7FF  && length > 2 ||
           codepoint <= 0xFFFF && length > 3  ;
}

static enum utf_error utf8_inc_strp(const char8_t **p_s)
{
    if (++*p_s == 0)
        return UTF_TRUNCATED;

    if ((**p_s & 0xC0) != 0x80)
        return UTF_TRUNCATED;

    return UTF_OK;
}

static enum utf_error utf8_getseq1(const char8_t **p_s, uint32_t *p_codepoint)
{
    if (**p_s == 0)
        return UTF_TRUNCATED;

    *p_codepoint = **p_s;

    return UTF_OK;
}

static enum utf_error utf8_getseq2(const char8_t **p_s, uint32_t *p_codepoint)
{
    if (**p_s == 0)
        return UTF_TRUNCATED;

    *p_codepoint = (**p_s & 0x1F) << 6;

    UTF8_TRY_INCREMENT_STRING_POINTER(p_s);
    *p_codepoint |= **p_s & 0x3F;

    return UTF_OK;
}

static enum utf_error utf8_getseq3(const char8_t **p_s, uint32_t *p_codepoint)
{
    if (**p_s == 0)
        return UTF_TRUNCATED;

    *p_codepoint = (**p_s & 0x0F) << 12;

    UTF8_TRY_INCREMENT_STRING_POINTER(p_s);
    *p_codepoint |= (**p_s & 0x3F) << 6;

    UTF8_TRY_INCREMENT_STRING_POINTER(p_s);
    *p_codepoint |= **p_s & 0x3F;

    return UTF_OK;
}

static enum utf_error utf8_getseq4(const char8_t **p_s, uint32_t *p_codepoint)
{
    if (**p_s == 0)
        return UTF_TRUNCATED;

    *p_codepoint = (**p_s & 0x07) << 18;

    UTF8_TRY_INCREMENT_STRING_POINTER(p_s);
    *p_codepoint |= (**p_s & 0x3F) << 12;

    UTF8_TRY_INCREMENT_STRING_POINTER(p_s);
    *p_codepoint |= (**p_s & 0x3F) << 6;

    UTF8_TRY_INCREMENT_STRING_POINTER(p_s);
    *p_codepoint |= **p_s & 0x3F;

    return UTF_OK;
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

enum utf_error utf8_validate_next(const char8_t **p_s, uint32_t *p_codepoint)
{
    if (**p_s == 0)
        return UTF_TRUNCATED;

    uint32_t cp = 0;
    int len = utf8_seqlen(*p_s);
    enum utf_error err = UTF_OK;

    switch (len) {
    case 0:
        return UTF_BAD_BYTE;
    case 1:
        err = utf8_getseq1(p_s, &cp);
        break;
    case 2:
        err = utf8_getseq2(p_s, &cp);
        break;
    case 3:
        err = utf8_getseq3(p_s, &cp);
        break;
    case 4:
        err = utf8_getseq4(p_s, &cp);
        break;
    }

    if (err) return err;

    if (utf8_is_overlong(cp, len))
        return UTF_OVERLONG;

    if (!isvalidunicode(cp))
        return UTF_BAD_CODEPOINT;

    *p_codepoint = cp;
    ++*p_s;

    return UTF_OK;
}
