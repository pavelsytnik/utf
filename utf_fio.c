#include "utf.h"

#include <stdlib.h>
#include <string.h>

#define utf_sequence_length_(c)  \
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

static bool utf_u8fread_bom_(struct utf_file *stream)
{
    if (getc(stream->file) == 0xEF &&
        getc(stream->file) == 0xBB &&
        getc(stream->file) == 0xBF)
        return true;

    rewind(stream);
    return false;
}

static enum utf_endianness utf_fread_bom_(struct utf_file *stream)
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

static const char *utf_mode_str_(enum utf_file_mode mode)
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

    c_file = fopen(filename, utf_mode_str_(mode));
    if (!c_file) { free(file); return NULL; }

    file->file = c_file;
    file->encoding = encoding;
    file->state = UTF_OK;

    if (encoding == UTF_U16 || encoding == UTF_U32)
        file->endianness = utf_fread_bom_(file);
    else if (encoding == UTF_U16_LE || encoding == UTF_U32_LE)
        file->endianness = UTF_LITTLE_ENDIAN;
    else if (encoding == UTF_U16_BE || encoding == UTF_U32_BE)
        file->endianness = UTF_BIG_ENDIAN;
    else
        file->endianness = utf_receive_endianness();

    if (encoding == UTF_U8_SIG)
        utf_u8fread_bom_(file);

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

static size_t utf_u8fnext_(utf_c8 *sym, struct utf_file *stream)
{
    int i, c;
    size_t len = 0;

    if ((c = getc(stream->file)) == EOF) {
        stream->state = UTF_OK;
        return 0;
    }

    sym[0] = (utf_c8)c;

    if ((len = utf_sequence_length_(c)) == 0) {
        stream->state = UTF_INVALID_LEAD;
        return 0;
    }

    for (i = 1; i < len; i++) {
        if ((c = getc(stream->file)) == EOF) {
            stream->state = UTF_NOT_ENOUGH_ROOM;
            return 0;
        }
        if ((c & 0xC0) != 0x80) {
            stream->state = UTF_INVALID_TRAIL;
            return 0;
        }

        sym[i] = (utf_c8)c;
    }

    if (sym[0] == 0xED && sym[1] > 0x9F ||
        sym[0] == 0xF4 && sym[1] > 0x8F ||
        sym[0] >  0xF4) {
        stream->state = UTF_INVALID_CODEPOINT;
        return 0;
    }
    if (sym[0] <  0xC2                  ||
        sym[0] == 0xE0 && sym[1] < 0xA0 ||
        sym[0] == 0xF0 && sym[1] < 0x90) {
        stream->state = UTF_OVERLONG_SEQUENCE;
        return 0;
    }

    stream->state = UTF_OK;
    return len;
}

static size_t utf_c16fread_(utf_c16 *restrict c,
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

static size_t utf_c32fread_(utf_c32 *restrict c,
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
    utf_c8 buf[4];
    size_t count;
    utf_c32 cp = 0;

    if ((count = utf_u8fnext_(buf, stream)) == 0)
        return UTF_EOF;

    switch (count) {
    case 1:
        cp |= (buf[0]);
        break;
    case 2:
        cp |= (buf[0] & 0x1F) << 6;
        cp |= (buf[1] & 0x3F);
        break;
    case 3:
        cp |= (buf[0] & 0x0F) << 12;
        cp |= (buf[1] & 0x3F) << 6;
        cp |= (buf[2] & 0x3F);
        break;
    case 4:
        cp |= (buf[0] & 0x07) << 18;
        cp |= (buf[1] & 0x3F) << 12;
        cp |= (buf[2] & 0x3F) << 6;
        cp |= (buf[3] & 0x3F);
        break;
    }

    return cp;
}

static size_t utf_u8fread_(utf_c8 *restrict buf,
                           size_t count,
                           struct utf_file *restrict stream)
{
    if (buf    == NULL ||
        count  == 0    ||
        stream == NULL)
        return 0;

    size_t read_chars = 0;

    while (count-- > 0) {
        utf_c8 c[4];
        size_t len;

        if ((len = utf_u8fnext_(c, stream)) == 0)
            break;

        memcpy(buf, c, len);
        buf += len;
        ++read_chars;
    }

    *buf = 0;
    return read_chars;
}

static size_t utf_u16fread_(utf_c16 *restrict buf,
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

        len = utf_c16fread_(c, stream);
        if (stream->state != UTF_OK || len == 0)
            break;

        memcpy(buf, c, len * 2);
        buf += len;
        ++read_chars;
    }

    *buf = 0;
    return read_chars;
}

static size_t utf_u32fread_(utf_c32 *restrict buf,
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

        if (!utf_c32fread_(c, stream))
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
        read = utf_u8fread_(buf, count, stream);
        break;
    case UTF_U16:
    case UTF_U16_LE:
    case UTF_U16_BE:
        chars = malloc(utf_s16sz(count));
        read = utf_u16fread_(chars, count, stream);
        utf_s16to8(buf, chars);
        break;
    case UTF_U32:
    case UTF_U32_LE:
    case UTF_U32_BE:
        chars = malloc(utf_s32sz(count));
        read = utf_u32fread_(chars, count, stream);
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
        read = utf_u8fread_(chars, count, stream);
        utf_s8to16(buf, chars);
        break;
    case UTF_U16:
    case UTF_U16_LE:
    case UTF_U16_BE:
        read = utf_u16fread_(buf, count, stream);
        break;
    case UTF_U32:
    case UTF_U32_LE:
    case UTF_U32_BE:
        chars = malloc(utf_s32sz(count));
        read = utf_u32fread_(chars, count, stream);
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
        read = utf_u8fread_(chars, count, stream);
        utf_s8to32(buf, chars);
        break;
    case UTF_U16:
    case UTF_U16_LE:
    case UTF_U16_BE:
        chars = malloc(utf_s16sz(count));
        read = utf_u16fread_(chars, count, stream);
        utf_s16to32(buf, chars);
        break;
    case UTF_U32:
    case UTF_U32_LE:
    case UTF_U32_BE:
        read = utf_u32fread_(buf, count, stream);
        break;
    }

    if (chars != NULL)
        free(chars);
    return read;
}
