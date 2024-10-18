#include "utf_fio.h"

#include "utf_byteorder.h"
#include "utf_strconv.h"

#include <stdlib.h>
#include <string.h>

struct utf_file {
    FILE *file;
    utf_file_encoding encoding;
    utf_error state;
    utf_endianness endianness;
};

static const char *utf_mode_str_(utf_file_mode mode);

static bool utf_8_fread_bom_(utf_file *stream);
static utf_endianness utf_fread_bom_(utf_file *stream);

static size_t utf_8_fnext_(utf_file *restrict stream,
                           utf_c8 *restrict c);
static size_t utf_16_fnext_(utf_file *restrict stream,
                            utf_c16 *restrict c);
static size_t utf_32_fnext_(utf_file *restrict stream,
                            utf_c32 *restrict c);

static utf_c32 utf_8_fgetc_(utf_file *stream);

static size_t utf_8_fread_(utf_file *stream,
                           utf_c8 *restrict buf,
                           size_t count);
static size_t utf_16_fread_(utf_file *stream,
                            utf_c16 *restrict buf,
                            size_t count);
static size_t utf_32_fread_(utf_file *stream,
                            utf_c32 *restrict buf,
                            size_t count);

static utf_error utf_8_strnext_(const utf_c8 *restrict str,
                                size_t *restrict dif);

static void utf_8_fputc_(utf_file *stream, utf_c32 code);

static size_t utf_8_fwrite_(utf_file *restrict stream,
                            const utf_c8 *restrict buffer,
                            size_t count);

utf_file *utf_fopen(const char *filename,
                    utf_file_mode mode,
                    utf_file_encoding encoding)
{
    utf_file *file;
    FILE *c_file;

    file = malloc(sizeof(utf_file));
    if (!file) return NULL;

    c_file = fopen(filename, utf_mode_str_(mode));
    if (!c_file) { free(file); return NULL; }

    file->file = c_file;
    file->encoding = encoding;
    file->state = UTF_OK;

    if (encoding == UTF_16 || encoding == UTF_32)
        file->endianness = utf_fread_bom_(file);
    else if (encoding == UTF_16_LE || encoding == UTF_32_LE)
        file->endianness = UTF_LITTLE_ENDIAN;
    else if (encoding == UTF_16_BE || encoding == UTF_32_BE)
        file->endianness = UTF_BIG_ENDIAN;
    else
        file->endianness = utf_system_endianness();

    if (encoding == UTF_8_SIG)
        utf_8_fread_bom_(file);

    return file;
}

bool utf_fclose(utf_file *stream)
{
    bool close_state = fclose(stream->file) == 0;
    free(stream);
    return close_state;
}

bool utf_feof(const utf_file *stream)
{
    return feof(stream->file) != 0;
}

FILE *utf_c_file(const utf_file *stream)
{
    return stream->file;
}

utf_error utf_ferror(const utf_file *stream)
{
    return stream->state;
}

size_t utf_fread(utf_file *restrict stream, void *restrict buf, size_t count)
{
    typedef size_t(*fread_ptr)(utf_file *, void *, size_t);
    static fread_ptr fread_table[] = {
        (fread_ptr)utf_8_fread_,
        (fread_ptr)utf_8_fread_,
        (fread_ptr)utf_16_fread_,
        (fread_ptr)utf_16_fread_,
        (fread_ptr)utf_16_fread_,
        (fread_ptr)utf_32_fread_,
        (fread_ptr)utf_32_fread_,
        (fread_ptr)utf_32_fread_
    };

    if (buf == NULL || count == 0 || stream == NULL)
        return 0;

    return fread_table[stream->encoding](stream, buf, count);
}

static const char *utf_mode_str_(utf_file_mode mode)
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

static bool utf_8_fread_bom_(utf_file *stream)
{
    if (getc(stream->file) == 0xEF &&
        getc(stream->file) == 0xBB &&
        getc(stream->file) == 0xBF)
        return true;

    rewind(stream);
    return false;
}

static utf_endianness utf_fread_bom_(utf_file *stream)
{
    uint8_t bytes[4] = {0, 0, 0, 0};

    switch (stream->encoding) {
    case UTF_16:
        fread(bytes, 1, 2, stream->file);
        if (bytes[0] == 0xFE &&
            bytes[1] == 0xFF)
            return UTF_BIG_ENDIAN;
        if (bytes[0] == 0xFF &&
            bytes[1] == 0xFE)
            return UTF_LITTLE_ENDIAN;
        break;
    case UTF_32:
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
    return utf_system_endianness();
}

static size_t utf_8_fnext_(utf_file *restrict stream, utf_c8 *restrict sym)
{
    int i, c;
    size_t len = 0;

    if ((c = getc(stream->file)) == EOF) {
        stream->state = UTF_OK;
        return 0;
    }

    sym[0] = (utf_c8)c;

    if ((len = utf_8_length_from_lead(c)) == 0) {
        stream->state = UTF_INVALID_LEAD;
        return 0;
    }

    for (i = 1; i < len; i++) {
        if ((c = getc(stream->file)) == EOF) {
            stream->state = UTF_TRUNCATED;
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
        stream->state = UTF_INVALID_CODE_POINT;
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

static size_t utf_16_fnext_(utf_file *restrict stream,
                            utf_c16 *restrict c)
{
    size_t bytes = fread(&c[0], 1, 2, stream->file);

    if (utf_feof(stream)) {
        stream->state = bytes == 0 ? UTF_OK : UTF_TRUNCATED;
        return 0;
    }

    if (utf_system_endianness() != stream->endianness)
        c[0] = utf_16_byteswap(c[0]);

    if (utf_is_trail_surrogate(c[0])) {
        stream->state = UTF_INVALID_LEAD;
        return 0;
    }

    if (utf_is_lead_surrogate(c[0])) {
        fread(&c[1], 1, 2, stream->file);

        if (utf_feof(stream)) {
            stream->state = UTF_TRUNCATED;
            return 0;
        }

        if (utf_system_endianness() != stream->endianness)
            c[1] = utf_16_byteswap(c[1]);

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

static size_t utf_32_fnext_(utf_file *restrict stream,
                            utf_c32 *restrict c)
{
    size_t bytes = fread(&c, 1, 4, stream->file);

    if (utf_feof(stream)) {
        stream->state = bytes == 0 ? UTF_OK : UTF_TRUNCATED;
        return 0;
    }

    if (utf_system_endianness() != stream->endianness)
        *c = utf_32_byteswap(*c);

    if (!utf_is_valid_code_point(*c)) {
        stream->state = UTF_INVALID_CODE_POINT;
        return 0;
    }

    stream->state = UTF_OK;
    return 1;
}

static utf_c32 utf_8_fgetc_(utf_file *stream)
{
    utf_c8 buf[4];
    size_t count;
    utf_c32 cp = 0;

    if ((count = utf_8_fnext_(buf, stream)) == 0)
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

static size_t utf_8_fread_(utf_file *stream,
                           utf_c8 *restrict buf,
                           size_t count)
{
    size_t read_chars = 0;

    while (count-- > 0) {
        utf_c8 c[4];
        size_t len;

        if ((len = utf_8_fnext_(stream, c)) == 0)
            break;

        memcpy(buf, c, len);
        buf += len;
        ++read_chars;
    }

    *buf = 0;
    return read_chars;
}

static size_t utf_16_fread_(utf_file *stream,
                            utf_c16 *restrict buf,
                            size_t count)
{
    size_t read_chars = 0;

    while (count-- > 0) {
        utf_c16 c[2];
        size_t len;

        len = utf_16_fnext_(stream, c);
        if (stream->state != UTF_OK || len == 0)
            break;

        memcpy(buf, c, len * 2);
        buf += len;
        ++read_chars;
    }

    *buf = 0;
    return read_chars;
}

static size_t utf_32_fread_(utf_file *stream,
                            utf_c32 *restrict buf,
                            size_t count)
{
    size_t read_chars = 0;

    while (count-- > 0) {
        utf_c32 c[1];

        if (!utf_32_fnext_(stream, c))
            break;

        *buf++ = c;
        ++read_chars;
    }

    *buf = 0;
    return read_chars;
}

static utf_error utf_8_strnext_(const utf_c8 *restrict str,
                                size_t *restrict dif)
{
    int i;

    if (utf_8_is_lead_1(*str)) {
        *dif = 1;
        return UTF_OK;
    }

    if (utf_8_is_lead_2(*str))
        *dif = 2;
    else if (utf_8_is_lead_3(*str))
        *dif = 3;
    else if (utf_8_is_lead_4(*str))
        *dif = 4;
    else
        return UTF_INVALID_LEAD;

    for (i = 1; i < *dif; i++)
        if (!utf_8_is_trail(*(str + i)))
            return UTF_INVALID_TRAIL;

    if (utf_8_is_invalid_code_point(str))
        return UTF_INVALID_CODE_POINT;
    if (utf_8_is_overlong_sequence(str))
        return UTF_OVERLONG_SEQUENCE;

    return UTF_OK;
}

static void utf_8_fputc_(utf_file *stream, utf_c32 code)
{
    if (code < 0x80) {
        putc(code,                     stream->file);
    } else if (code < 0x800) {
        putc(code >> 6         | 0xC0, stream->file);
        putc(code       & 0x3F | 0x80, stream->file);
    } else if (code < 0x10000) {
        putc(code >> 12        | 0xE0, stream->file);
        putc(code >> 6  & 0x3F | 0x80, stream->file);
        putc(code       & 0x3F | 0x80, stream->file);
    } else if (code < 0x110000) {
        putc(code >> 18        | 0xF0, stream->file);
        putc(code >> 12 & 0x3F | 0x80, stream->file);
        putc(code >> 6  & 0x3F | 0x80, stream->file);
        putc(code       & 0x3F | 0x80, stream->file);
    }
}

static size_t utf_8_fwrite_(utf_file *restrict stream,
                            const utf_c8 *restrict buffer,
                            size_t count)
{
    size_t n = 0;

    while (count-- > 0) {
        size_t len;

        if ((stream->state = utf_8_strnext_(buffer, &len)) != UTF_OK)
            break;

        fwrite(buffer, 1, len, stream->file);
        buffer += len;
        n++;
    }

    return n;
}
