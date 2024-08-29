#include "utf.h"

#include <stdlib.h>
#include <string.h>

#define UTF_NEXT_TRAIL_OR_FAIL(c, stream) do { \
    if ((*(c) = getc(stream)) == EOF)          \
        return UTF_NOT_ENOUGH_ROOM;            \
    if ((*(c) & 0xC0) != 0x80)                 \
        return UTF_INVALID_TRAIL;              \
} while (0)

#define UTF_IS_OVERLONG_SEQUENCE(codepoint, length) \
    ((codepoint) <= 0x7F   && (length) > 1 ||       \
     (codepoint) <= 0x7FF  && (length) > 2 ||       \
     (codepoint) <= 0xFFFF && (length) > 3 )

#define UTF_SEQUENCE_LENGTH(c)  \
    (((c) & 0x80) == 0x00 ? 1 : \
     ((c) & 0xE0) == 0xC0 ? 2 : \
     ((c) & 0xF0) == 0xE0 ? 3 : \
     ((c) & 0xF8) == 0xF0 ? 4 : \
                            0 )

struct utf_file {
    enum utf_file_encoding encoding;
    enum utf_error state;
    FILE *file;
};

static bool utf_internal_fread_bom(struct utf_file *stream)
{
    char32_t sym[UTF_U32_ARRSZ(1)];
    utf_u32fread(sym, 1, stream);

    if (sym[0] == 0xFEFF)
        return true;

    rewind(stream->file);
    stream->state = UTF_OK;
    return false;
}

static const char *utf_mode_str(enum utf_file_mode mode)
{
    switch (mode) {
        case UTF_READ:
            return "r";
        case UTF_WRITE:
            return "w";
        case UTF_WRITE | UTF_APPEND:
        case UTF_APPEND:
            return "a";
        case UTF_READ | UTF_APPEND:
            return "r+";
        case UTF_READ | UTF_WRITE:
            return "w+";
        case UTF_READ | UTF_WRITE | UTF_APPEND:
            return "a+";
        case UTF_READ | UTF_BINARY:
            return "rb";
        case UTF_WRITE | UTF_BINARY:
            return "wb";
        case UTF_WRITE | UTF_APPEND | UTF_BINARY:
        case UTF_APPEND | UTF_BINARY:
            return "ab";
        case UTF_READ | UTF_APPEND | UTF_BINARY:
            return "r+b";
        case UTF_READ | UTF_WRITE | UTF_BINARY:
            return "w+b";
        case UTF_READ | UTF_WRITE | UTF_APPEND | UTF_BINARY:
            return "a+b";
        default:
            return NULL;
    }
}

struct utf_file *utf_fopen(const char *filename,
                           enum utf_file_mode mode,
                           enum utf_file_encoding encoding)
{
    struct utf_file *file = malloc(sizeof(struct utf_file));
    if (!file) return NULL;

    FILE *c_file = fopen(filename, utf_mode_str(mode));
    if (!c_file) { free(file); return NULL; }

    file->encoding = encoding;
    file->state = UTF_OK;
    file->file = c_file;

    if (encoding != UTF_U8)
        utf_internal_fread_bom(file);

    return file;
}

bool utf_fclose(struct utf_file *stream)
{
    bool close_state = fclose(stream->file) == 0;
    free(stream);
    return close_state;
}

bool utf_eof(const struct utf_file *stream)
{
    return feof(stream->file) != 0;
}

FILE *utf_c_file(const struct utf_file *stream)
{
    return stream->file;
}

enum utf_error utf_ferror(const struct utf_file *stream)
{
    return stream->state;
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

static enum utf_error utf_fread_sequence(char8_t *sequence,
                                         size_t *length,
                                         FILE *stream)
{
    int c = fgetc(stream);
    uint32_t codepoint = 0;

    if (feof(stream)) {
        *length = 0;
        return UTF_OK;
    }

    switch (*length = UTF_SEQUENCE_LENGTH(c)) {
    case 1:
        sequence[0] = c;
        codepoint |= c;

        break;
    case 2:
        sequence[0] = c;
        codepoint |= (c & 0x1F) << 6;

        UTF_NEXT_TRAIL_OR_FAIL(&c, stream);
        sequence[1] = c;
        codepoint |= (c & 0x3F);

        break;
    case 3:
        sequence[0] = c;
        codepoint |= (c & 0x0F) << 12;

        UTF_NEXT_TRAIL_OR_FAIL(&c, stream);
        sequence[1] = c;
        codepoint |= (c & 0x3F) << 6;

        UTF_NEXT_TRAIL_OR_FAIL(&c, stream);
        sequence[2] = c;
        codepoint |= (c & 0x3F);

        break;
    case 4:
        sequence[0] = c;
        codepoint |= (c & 0x07) << 18;

        UTF_NEXT_TRAIL_OR_FAIL(&c, stream);
        sequence[1] = c;
        codepoint |= (c & 0x3F) << 12;

        UTF_NEXT_TRAIL_OR_FAIL(&c, stream);
        sequence[2] = c;
        codepoint |= (c & 0x3F) << 6;

        UTF_NEXT_TRAIL_OR_FAIL(&c, stream);
        sequence[3] = c;
        codepoint |= (c & 0x3F);

        break;
    default:
        return UTF_INVALID_LEAD;
    }

    if (!UTF_IS_VALID_CODEPOINT(codepoint))
        return UTF_INVALID_CODEPOINT;
    if (UTF_IS_OVERLONG_SEQUENCE(codepoint, *length))
        return UTF_OVERLONG_SEQUENCE;

    return UTF_OK;
}

static size_t utf_internal_c16fread(char16_t *restrict c,
                                    struct utf_file *restrict stream)
{
    size_t bytes = fread(&c[0], 1, 2, stream->file);

    if (utf_eof(stream)) {
        stream->state = bytes == 0 ? UTF_OK : UTF_NOT_ENOUGH_ROOM;
        return 0;
    }

    if (UTF_IS_TRAIL_SURROGATE(c[0])) {
        stream->state = UTF_INVALID_LEAD;
        return 0;
    }

    if (UTF_IS_LEAD_SURROGATE(c[0])) {
        fread(&c[1], 1, 2, stream->file);

        if (utf_eof(stream)) {
            stream->state = UTF_NOT_ENOUGH_ROOM;
            return 0;
        }
        if (!UTF_IS_TRAIL_SURROGATE(c[1])) {
            stream->state = UTF_INVALID_TRAIL;
            return 0;
        }

        stream->state = UTF_OK;
        return 2;
    }

    stream->state = UTF_OK;
    return 1;
}

static size_t utf_internal_c32fread(char32_t *restrict c,
                                    struct utf_file *restrict stream)
{
    size_t bytes = fread(&c, 1, 4, stream->file);

    if (utf_eof(stream)) {
        stream->state = bytes == 0 ? UTF_OK : UTF_NOT_ENOUGH_ROOM;
        return 0;
    }

    if (!UTF_IS_VALID_CODEPOINT(*c)) {
        stream->state = UTF_INVALID_CODEPOINT;
        return 0;
    }

    stream->state = UTF_OK;
    return 1;
}

// TODO: refactor internal logic
uint32_t utf_u8getc_s(FILE *stream, enum utf_error *err)
{
    int c;
    uint32_t cp;
    size_t len;

    if ((c = getc(stream)) == EOF) {
        *err = UTF_OK;
        return UTF_EOF;
    }

    switch (len = UTF_SEQUENCE_LENGTH(c)) {
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
        return UTF_EOF;
    }

    while (--len) {
        if ((c = getc(stream)) == EOF) {
            *err = UTF_NOT_ENOUGH_ROOM;
            return UTF_EOF;
        }
        if ((c & 0xC0) != 0x80) {
            *err = UTF_INVALID_TRAIL;
            return UTF_EOF;
        }
        cp = cp << 6 | c & 0x3F;
    }

    if (!UTF_IS_VALID_CODEPOINT(cp)) {
        *err = UTF_INVALID_CODEPOINT;
        return UTF_EOF;
    }
    if (UTF_IS_OVERLONG_SEQUENCE(cp, len)) {
        *err = UTF_OVERLONG_SEQUENCE;
        return UTF_EOF;
    }

    *err = UTF_OK;
    return cp;
}

static size_t utf_internal_u8fread(char8_t *restrict buf,
                                   size_t count,
                                   struct utf_file *restrict stream)
{
    if (buf    == NULL ||
        count  == 0    ||
        stream == NULL)
        return 0;

    size_t read_chars = 0;
    enum utf_error stat = UTF_OK;

    while (count-- > 0) {
        char8_t c[4];
        size_t len;

        stat = utf_fread_sequence(c, &len, stream->file);
        if (stat != UTF_OK || len == 0)
            break;

        memcpy(buf, c, len);
        buf += len;
        ++read_chars;
    }

    stream->state = stat;
    *buf = 0;
    return read_chars;
}

static size_t utf_internal_u16fread(char16_t *restrict buf,
                                    size_t count,
                                    struct utf_file *restrict stream)
{
    if (buf    == NULL ||
        count  == 0    ||
        stream == NULL)
        return 0;

    size_t read_chars = 0;

    while (count-- > 0) {
        char16_t c[2];
        size_t len;

        len = utf_internal_c16fread(c, stream);
        if (stream->state != UTF_OK || len == 0)
            break;

        memcpy(buf, c, len * 2);
        buf += len;
        ++read_chars;
    }

    *buf = 0;
    return read_chars;
}

static size_t utf_internal_u32fread(char32_t *restrict buf,
                                    size_t count,
                                    struct utf_file *restrict stream)
{
    if (buf    == NULL ||
        count  == 0    ||
        stream == NULL)
        return 0;

    size_t read_chars = 0;

    while (count-- > 0) {
        char32_t c[1];

        if (!utf_internal_c32fread(c, stream))
            break;

        *buf++ = c;
        ++read_chars;
    }

    *buf = 0;
    return read_chars;
}

size_t utf_u8fread(char8_t *restrict buf,
                   size_t count,
                   struct utf_file *restrict stream)
{
    size_t read = 0;
    void *chars = NULL;

    switch (stream->encoding)
    {
    case UTF_U8:
        read = utf_internal_u8fread(buf, count, stream);
        break;
    case UTF_U16:
        chars = malloc(UTF_U16_SZ(count));
        read = utf_internal_u16fread(chars, count, stream);
        utf_str16to8(buf, chars);
        break;
    case UTF_U32:
        chars = malloc(UTF_U32_SZ(count));
        read = utf_internal_u32fread(chars, count, stream);
        utf_str32to8(buf, chars);
        break;
    }

    if (chars != NULL)
        free(chars);
    return read;
}

size_t utf_u16fread(char16_t *restrict buf,
                    size_t count,
                    struct utf_file *restrict stream)
{
    size_t read = 0;
    void *chars = NULL;

    switch (stream->encoding)
    {
    case UTF_U8:
        chars = malloc(UTF_U8_SZ(count));
        read = utf_internal_u8fread(chars, count, stream);
        utf_str8to16(buf, chars);
        break;
    case UTF_U16:
        read = utf_internal_u16fread(buf, count, stream);
        break;
    case UTF_U32:
        chars = malloc(UTF_U32_SZ(count));
        read = utf_internal_u32fread(chars, count, stream);
        utf_str32to16(buf, chars);
        break;
    }

    if (chars != NULL)
        free(chars);
    return read;
}

size_t utf_u32fread(char32_t *restrict buf,
                    size_t count,
                    struct utf_file *restrict stream)
{
    size_t read = 0;
    void *chars = NULL;

    switch (stream->encoding) {
    case UTF_U8:
        chars = malloc(UTF_U8_SZ(count));
        read = utf_internal_u8fread(chars, count, stream);
        utf_str8to32(buf, chars);
        break;
    case UTF_U16:
        chars = malloc(UTF_U16_SZ(count));
        read = utf_internal_u16fread(chars, count, stream);
        utf_str16to32(buf, chars);
        break;
    case UTF_U32:
        read = utf_internal_u32fread(buf, count, stream);
        break;
    }

    if (chars != NULL)
        free(chars);
    return read;
}
