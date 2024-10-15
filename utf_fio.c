#include "utf.h"

#include <stdlib.h>
#include <string.h>

#define UTF_NEXT_TRAIL_OR_FAIL(c, stream) \
    if ((*(c) = getc(stream)) == EOF)     \
        return UTF_NOT_ENOUGH_ROOM;       \
    if ((*(c) & 0xC0) != 0x80)            \
        return UTF_INVALID_TRAIL;

#define utf_is_overlong_sentence(codepoint, length) \
    ((codepoint) <= 0x7F   && (length) > 1 ||       \
     (codepoint) <= 0x7FF  && (length) > 2 ||       \
     (codepoint) <= 0xFFFF && (length) > 3 )

#define utf_sequence_length(c)  \
    (((c) & 0x80) == 0x00 ? 1 : \
     ((c) & 0xE0) == 0xC0 ? 2 : \
     ((c) & 0xF0) == 0xE0 ? 3 : \
     ((c) & 0xF8) == 0xF0 ? 4 : \
                            0 )

struct utf_file {
    FILE *file;
    enum utf_file_encoding encoding;
    enum utf_error state;
    enum utf_endianness endianness;
};

static bool utf_u8fread_bom(struct utf_file *stream)
{
    if (getc(stream->file) == 0xEF &&
        getc(stream->file) == 0xBB &&
        getc(stream->file) == 0xBF)
        return true;

    rewind(stream);
    return false;
}

static enum utf_endianness utf_fread_bom(struct utf_file *stream)
{
    uint8_t bytes[4] = { 0, 0, 0, 0 };

    switch (stream->encoding) {
    case UTF_U16:
        fread(bytes, 1, 2, stream->file);
        if (bytes[0] == 0xFE &&
            bytes[1] == 0xFF)
            return UTF_BIG_ENDIAN;
        if (bytes[0] == 0xFF &&
            bytes[1] == 0xFE)
            return UTF_LITTLE_ENDIAN;
        break;
    case UTF_U32:
        fread(bytes, 1, 4, stream->file);
        if (bytes[0] == 0x00 &&
            bytes[1] == 0x00 &&
            bytes[2] == 0xFE &&
            bytes[3] == 0xFF)
            return UTF_BIG_ENDIAN;
        if (bytes[0] == 0xFF &&
            bytes[1] == 0xFE &&
            bytes[2] == 0x00 &&
            bytes[3] == 0x00)
            return UTF_LITTLE_ENDIAN;
        break;
    }

    rewind(stream->file);
    return utf_receive_endianness();
}

static const char *utf_mode_str(enum utf_file_mode mode)
{
    switch (mode) {
        case UTF_READ:
            return "rb";
        case UTF_WRITE:
            return "wb";
        case UTF_WRITE | UTF_APPEND:
        case UTF_APPEND:
            return "ab";
        case UTF_READ | UTF_APPEND:
            return "r+b";
        case UTF_READ | UTF_WRITE:
            return "w+b";
        case UTF_READ | UTF_WRITE | UTF_APPEND:
            return "a+b";
        default:
            return NULL;
    }
}

struct utf_file *utf_fopen(const char *filename,
                           enum utf_file_mode mode,
                           enum utf_file_encoding encoding)
{
    struct utf_file *file;
    FILE *c_file;

    file = malloc(sizeof(struct utf_file));
    if (!file) return NULL;

    c_file = fopen(filename, utf_mode_str(mode));
    if (!c_file) { free(file); return NULL; }

    file->file = c_file;
    file->encoding = encoding;
    file->state = UTF_OK;

    if (encoding == UTF_U16 || encoding == UTF_U32)
        file->endianness = utf_fread_bom(file);
    else if (encoding == UTF_U16_LE || encoding == UTF_U32_LE)
        file->endianness = UTF_LITTLE_ENDIAN;
    else if (encoding == UTF_U16_BE || encoding == UTF_U32_BE)
        file->endianness = UTF_BIG_ENDIAN;
    else
        file->endianness = utf_receive_endianness();

    if (encoding == UTF_U8_SIG)
        utf_u8fread_bom(file);

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

/* TODO: use the algorithm somewhere else or permanently delete */
//int utf_u8getc(utf_c8 *bytes, FILE *stream)
//{
//    int c, contbytes, nbytes = 0;
//
//    if ((c = getc(stream)) == EOF)
//        return 0;
//
//    bytes[0] = (utf_c8) c;
//    nbytes++;
//
//    if ((c & 0x80) == 0x00)
//        contbytes = 0;
//    else if ((c & 0xE0) == 0xC0)
//        contbytes = 1;
//    else if ((c & 0xF0) == 0xE0)
//        contbytes = 2;
//    else if ((c & 0xF8) == 0xF0)
//        contbytes = 3;
//    else
//        return -1;
//
//    while (contbytes--) {
//        if ((c = getc(stream)) == EOF)
//            return -nbytes;
//        bytes[nbytes++] = (utf_c8) c;
//        if ((c & 0xC0) != 0x80)
//            return -nbytes;
//    }
//
//    return nbytes;
//}

static enum utf_error utf_fread_sequence(utf_c8 *sequence,
                                         size_t *length,
                                         FILE *stream)
{
    int c = fgetc(stream);
    uint32_t codepoint = 0;

    if (feof(stream)) {
        *length = 0;
        return UTF_OK;
    }

    switch (*length = utf_sequence_length(c)) {
    case 1:
        sequence[0] = c;
        codepoint |= c;

        break;
    case 2:
        sequence[0] = c;
        codepoint |= (c & 0x1F) << 6;

        UTF_NEXT_TRAIL_OR_FAIL(&c, stream)
        sequence[1] = c;
        codepoint |= (c & 0x3F);

        break;
    case 3:
        sequence[0] = c;
        codepoint |= (c & 0x0F) << 12;

        UTF_NEXT_TRAIL_OR_FAIL(&c, stream)
        sequence[1] = c;
        codepoint |= (c & 0x3F) << 6;

        UTF_NEXT_TRAIL_OR_FAIL(&c, stream)
        sequence[2] = c;
        codepoint |= (c & 0x3F);

        break;
    case 4:
        sequence[0] = c;
        codepoint |= (c & 0x07) << 18;

        UTF_NEXT_TRAIL_OR_FAIL(&c, stream)
        sequence[1] = c;
        codepoint |= (c & 0x3F) << 12;

        UTF_NEXT_TRAIL_OR_FAIL(&c, stream)
        sequence[2] = c;
        codepoint |= (c & 0x3F) << 6;

        UTF_NEXT_TRAIL_OR_FAIL(&c, stream)
        sequence[3] = c;
        codepoint |= (c & 0x3F);

        break;
    default:
        return UTF_INVALID_LEAD;
    }

    if (!utf_is_valid_codepoint(codepoint))
        return UTF_INVALID_CODEPOINT;
    if (utf_is_overlong_sentence(codepoint, *length))
        return UTF_OVERLONG_SEQUENCE;

    return UTF_OK;
}

static size_t utf_internal_c16fread(utf_c16 *restrict c,
                                    struct utf_file *restrict stream)
{
    size_t bytes = fread(&c[0], 1, 2, stream->file);

    if (utf_eof(stream)) {
        stream->state = bytes == 0 ? UTF_OK : UTF_NOT_ENOUGH_ROOM;
        return 0;
    }

    if (utf_receive_endianness() != stream->endianness)
        c[0] = utf_swapbytes_uint16(c[0]);

    if (utf_is_trail_surrogate(c[0])) {
        stream->state = UTF_INVALID_LEAD;
        return 0;
    }

    if (utf_is_lead_surrogate(c[0])) {
        fread(&c[1], 1, 2, stream->file);

        if (utf_eof(stream)) {
            stream->state = UTF_NOT_ENOUGH_ROOM;
            return 0;
        }

        if (utf_receive_endianness() != stream->endianness)
            c[1] = utf_swapbytes_uint16(c[1]);

        if (!utf_is_trail_surrogate(c[1])) {
            stream->state = UTF_INVALID_TRAIL;
            return 0;
        }

        stream->state = UTF_OK;
        return 2;
    }

    stream->state = UTF_OK;
    return 1;
}

static size_t utf_internal_c32fread(utf_c32 *restrict c,
                                    struct utf_file *restrict stream)
{
    size_t bytes = fread(&c, 1, 4, stream->file);

    if (utf_eof(stream)) {
        stream->state = bytes == 0 ? UTF_OK : UTF_NOT_ENOUGH_ROOM;
        return 0;
    }

    if (utf_receive_endianness() != stream->endianness)
        *c = utf_swapbytes_uint32(*c);

    if (!utf_is_valid_codepoint(*c)) {
        stream->state = UTF_INVALID_CODEPOINT;
        return 0;
    }

    stream->state = UTF_OK;
    return 1;
}

utf_c32 utf_fgetc8(struct utf_file *stream)
{
    int i, c;
    utf_c32 cp;
    size_t len;

    if ((c = getc(stream->file)) == EOF) {
        stream->state = UTF_OK;
        return UTF_EOF;
    }

    if ((c & 0x80) == 0x00) {
        stream->state = UTF_OK;
        return (utf_c32)c;
    } else if ((c & 0xE0) == 0xC0) {
        len = 2;
        cp = (utf_c32)(c & 0x1F);
    } else if ((c & 0xF0) == 0xE0) {
        len = 3;
        cp = (utf_c32)(c & 0x0F);
    } else if ((c & 0xF8) == 0xF0) {
        len = 4;
        cp = (utf_c32)(c & 0x07);
    } else {
        stream->state = UTF_INVALID_LEAD;
        return UTF_EOF;
    }

    for (i = 1; i < len; i++) {
        if ((c = getc(stream->file)) == EOF) {
            stream->state = UTF_NOT_ENOUGH_ROOM;
            return UTF_EOF;
        }
        if ((c & 0xC0) != 0x80) {
            stream->state = UTF_INVALID_TRAIL;
            return UTF_EOF;
        }

        cp = cp << 6 | (utf_c32)(c & 0x3F);
    }

    if (!utf_is_valid_codepoint(cp)) {
        stream->state = UTF_INVALID_CODEPOINT;
        return UTF_EOF;
    }
    if (utf_is_overlong_sentence(cp, len)) {
        stream->state = UTF_OVERLONG_SEQUENCE;
        return UTF_EOF;
    }

    stream->state = UTF_OK;
    return cp;
}

static size_t utf_internal_u8fread(utf_c8 *restrict buf,
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
        utf_c8 c[4];
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

static size_t utf_internal_u16fread(utf_c16 *restrict buf,
                                    size_t count,
                                    struct utf_file *restrict stream)
{
    if (buf    == NULL ||
        count  == 0    ||
        stream == NULL)
        return 0;

    size_t read_chars = 0;

    while (count-- > 0) {
        utf_c16 c[2];
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

static size_t utf_internal_u32fread(utf_c32 *restrict buf,
                                    size_t count,
                                    struct utf_file *restrict stream)
{
    if (buf    == NULL ||
        count  == 0    ||
        stream == NULL)
        return 0;

    size_t read_chars = 0;

    while (count-- > 0) {
        utf_c32 c[1];

        if (!utf_internal_c32fread(c, stream))
            break;

        *buf++ = c;
        ++read_chars;
    }

    *buf = 0;
    return read_chars;
}

size_t utf_u8fread(utf_c8 *restrict buf,
                   size_t count,
                   struct utf_file *restrict stream)
{
    size_t read = 0;
    void *chars = NULL;

    switch (stream->encoding) {
    case UTF_U8:
    case UTF_U8_SIG:
        read = utf_internal_u8fread(buf, count, stream);
        break;
    case UTF_U16:
    case UTF_U16_LE:
    case UTF_U16_BE:
        chars = malloc(utf_s16sz(count));
        read = utf_internal_u16fread(chars, count, stream);
        utf_s16to8(buf, chars);
        break;
    case UTF_U32:
    case UTF_U32_LE:
    case UTF_U32_BE:
        chars = malloc(utf_s32sz(count));
        read = utf_internal_u32fread(chars, count, stream);
        utf_s32to8(buf, chars);
        break;
    }

    if (chars != NULL)
        free(chars);
    return read;
}

size_t utf_u16fread(utf_c16 *restrict buf,
                    size_t count,
                    struct utf_file *restrict stream)
{
    size_t read = 0;
    void *chars = NULL;

    switch (stream->encoding) {
    case UTF_U8:
    case UTF_U8_SIG:
        chars = malloc(utf_s8sz(count));
        read = utf_internal_u8fread(chars, count, stream);
        utf_s8to16(buf, chars);
        break;
    case UTF_U16:
    case UTF_U16_LE:
    case UTF_U16_BE:
        read = utf_internal_u16fread(buf, count, stream);
        break;
    case UTF_U32:
    case UTF_U32_LE:
    case UTF_U32_BE:
        chars = malloc(utf_s32sz(count));
        read = utf_internal_u32fread(chars, count, stream);
        utf_s32to16(buf, chars);
        break;
    }

    if (chars != NULL)
        free(chars);
    return read;
}

size_t utf_u32fread(utf_c32 *restrict buf,
                    size_t count,
                    struct utf_file *restrict stream)
{
    size_t read = 0;
    void *chars = NULL;

    switch (stream->encoding) {
    case UTF_U8:
    case UTF_U8_SIG:
        chars = malloc(utf_s8sz(count));
        read = utf_internal_u8fread(chars, count, stream);
        utf_s8to32(buf, chars);
        break;
    case UTF_U16:
    case UTF_U16_LE:
    case UTF_U16_BE:
        chars = malloc(utf_s16sz(count));
        read = utf_internal_u16fread(chars, count, stream);
        utf_s16to32(buf, chars);
        break;
    case UTF_U32:
    case UTF_U32_LE:
    case UTF_U32_BE:
        read = utf_internal_u32fread(buf, count, stream);
        break;
    }

    if (chars != NULL)
        free(chars);
    return read;
}
