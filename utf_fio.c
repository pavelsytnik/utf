#include "utf.h"

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

char8_t *utf_u8fread(char8_t *buf, size_t count, FILE *stream)
{
    if (count == 0) return NULL;

    while (count > 1) {
        int ch = getc(stream);
        if (ch == EOF)
            break;

        int seqlen;
        if ((ch & 0x80) == 0x00)
            seqlen = 1;
        else if ((ch & 0xE0) == 0xC0)
            seqlen = 2;
        else if ((ch & 0xF0) == 0xE0)
            seqlen = 3;
        else if ((ch & 0xF8) == 0xF0)
            seqlen = 4;
        else
            break;

        if (seqlen > count - 1)
            break;

        *buf++ = ch;
        count--;

        for (int i = 1; i < seqlen; ++i) {
            ch = getc(stream);
            if (ch == EOF || (ch & 0xC0) != 0x80) {
                buf -= i;
                goto end;
            }
            *buf++ = ch;
            count--;
        }
    }
    end:
    *buf = 0;
    return buf;
}

int utf_u8getc(char8_t *bytes, FILE *stream)
{
    int c, contbytes, nbytes = 0;

    if ((c = getc(stream)) == EOF)
        return 0;

    bytes[0] = (char8_t) c;
    nbytes++;

    if ((c & 0x80) == 0x00)
        contbytes = 0;
    else if ((c & 0xE0) == 0xC0)
        contbytes = 1;
    else if ((c & 0xF0) == 0xE0)
        contbytes = 2;
    else if ((c & 0xF8) == 0xF0)
        contbytes = 3;
    else
        return -1;

    while (contbytes--) {
        if ((c = getc(stream)) == EOF)
            return -nbytes;
        bytes[nbytes++] = (char8_t) c;
        if ((c & 0xC0) != 0x80)
            return -nbytes;
    }

    return nbytes;
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
    if (buf == NULL ||
        count == 0 ||
        stream == NULL ||
        err == NULL)
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
