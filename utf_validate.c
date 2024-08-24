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

static enum utf_error utf_decode_sequence(const char8_t **strp,
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

enum utf_error utf_u8next(const char8_t **strp, uint32_t *codepoint)
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
    else if (!UTF_IS_VALID_CODEPOINT(cp))
        err = UTF_INVALID_CODEPOINT;

    if (err != UTF_OK) {
        *strp -= len - 1;
        return err;
    }

    *codepoint = cp;
    ++*strp;

    return UTF_OK;
}

enum utf_error utf_u16next(const char16_t **strp, uint32_t *codepoint)
{
    if (**strp == 0)
        return UTF_NOT_ENOUGH_ROOM;

    uint32_t cp = 0;

    if (!UTF_IS_SURROGATE(**strp)) {
        cp = **strp;
    } else if (UTF_IS_LEAD_SURROGATE(**strp)) {
        cp = **strp - UTF_LEAD_SURROGATE_MIN << 10;

        if (*++*strp == 0)
            return UTF_NOT_ENOUGH_ROOM;
        if (!UTF_IS_TRAIL_SURROGATE(**strp))
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

// The function has to be within 'utf_fio.c'
// TODO: refactor internal logic
uint32_t utf_u8getc_s(FILE *stream, enum utf_error *err)
{
    int c;
    uint32_t cp;
    int len;

    if ((c = getc(stream)) == EOF) {
        *err = UTF_OK;
        return 0xFFFFFFFF;
    }

    len = UTF_SEQUENCE_LENGTH(c);
    switch (len) {
    case 1:
        cp = c;
        break;
    case 2:
        cp = c & 0x1F;
        break;
    case 3:
        cp = c & 0x0F;
        break;
    case 4:
        cp = c & 0x07;
    default:
        *err = UTF_INVALID_LEAD;
        return 0xFFFFFFFF;
    }

    while (--len) {
        if ((c = getc(stream)) == EOF) {
            *err = UTF_NOT_ENOUGH_ROOM;
            return 0xFFFFFFFF;
        }
        if ((c & 0xC0) != 0x80) {
            *err = UTF_INVALID_TRAIL;
            return 0xFFFFFFFF;
        }
        cp = cp << 6 | c & 0x3F;
    }

    if (!UTF_IS_VALID_CODEPOINT(cp)) {
        *err = UTF_INVALID_CODEPOINT;
        return 0xFFFFFFFF;
    }
    if (UTF_IS_OVERLONG_SEQUENCE(cp, len)) {
        *err = UTF_OVERLONG_SEQUENCE;
        return 0xFFFFFFFF;
    }

    *err = UTF_OK;
    return cp;
}

// TODO: get rid of extra conversions
size_t utf_u8fread_s(char8_t *buf,
                     size_t count,
                     FILE *stream,
                     enum utf_error *err)
{
    if (buf    == NULL ||
        count  == 0    ||
        stream == NULL ||
        err    == NULL   )
        return 0;

    size_t read_chars = 0;

    *err = UTF_OK;

    while (count-- > 0) {
        enum utf_error stat;
        uint32_t cp = utf_u8getc_s(stream, &stat);

        if (cp == 0xFFFFFFFF) {
            *err = stat;
            break;
        }

        if (cp <= 0x7F) {
            *buf++ = cp;
        } else if (cp <= 0x7FF) {
            *buf++ = cp >> 6 | 0xC0;
            *buf++ = cp & 0x3F | 0x80;
        } else if (cp <= 0xFFFF) {
            *buf++ = cp >> 12 | 0xE0;
            *buf++ = cp >> 6 & 0x3F | 0x80;
            *buf++ = cp & 0x3F | 0x80;
        } else if (cp <= 0x10FFFF) {
            *buf++ = cp >> 18 | 0xF0;
            *buf++ = cp >> 12 & 0x3F | 0x80;
            *buf++ = cp >> 6 & 0x3F | 0x80;
            *buf++ = cp & 0x3F | 0x80;
        }

        ++read_chars;
    }

    *buf = 0;
    return read_chars;
}
