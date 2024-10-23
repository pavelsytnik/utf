#include "utf_fio.h"

#include "utf_byteorder.h"
#include "utf_chrconv.h"
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

static utf_bool utf_8_fread_bom_(utf_file *stream);
static utf_endianness utf_fread_bom_(utf_file *stream);

static size_t utf_8_fread_next_raw_(utf_file *UTF_RESTRICT stream,
                                    utf_c8 *UTF_RESTRICT c);
static size_t utf_16_fread_next_raw_(utf_file *UTF_RESTRICT stream,
                                     utf_c16 *UTF_RESTRICT c);
static size_t utf_32_fread_next_raw_(utf_file *UTF_RESTRICT stream,
                                     utf_c32 *UTF_RESTRICT c);

static utf_bool utf_8_fread_next_(utf_file *UTF_RESTRICT stream,
                                  utf_c8 *UTF_RESTRICT c);
static utf_bool utf_16_fread_next_(utf_file *UTF_RESTRICT stream,
                                   utf_c8 *UTF_RESTRICT c);
static utf_bool utf_32_fread_next_(utf_file *UTF_RESTRICT stream,
                                   utf_c8 *UTF_RESTRICT c);

static utf_c32 utf_8_fgetc_(utf_file *stream);
static utf_c32 utf_16_fgetc_(utf_file *stream);
static utf_c32 utf_32_fgetc_(utf_file *stream);

static void utf_8_fwrite_next_(utf_file *UTF_RESTRICT stream,
                               const utf_c8 *UTF_RESTRICT c);
static void utf_16_fwrite_next_(utf_file *UTF_RESTRICT stream,
                                const utf_c8 *UTF_RESTRICT c);
static void utf_32_fwrite_next_(utf_file *UTF_RESTRICT stream,
                                const utf_c8 *UTF_RESTRICT c);

static void utf_8_fputc_(utf_file *stream, utf_c32 code);
static void utf_16_fputc_(utf_file *stream, utf_c32 code);
static void utf_32_fputc_(utf_file *stream, utf_c32 code);

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

utf_bool utf_fclose(utf_file *stream)
{
    utf_bool close_state = fclose(stream->file) == 0;
    free(stream);
    return close_state;
}

utf_bool utf_feof(const utf_file *stream)
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

utf_c32 utf_fgetc(utf_file *stream)
{
    typedef utf_c32 (*fgetc_ptr)(utf_file *);
    static fgetc_ptr fgetc_table[] = {
        utf_8_fgetc_,
        utf_8_fgetc_,
        utf_16_fgetc_,
        utf_16_fgetc_,
        utf_16_fgetc_,
        utf_32_fgetc_,
        utf_32_fgetc_,
        utf_32_fgetc_
    };

    return fgetc_table[stream->encoding](stream);
}

utf_c32 utf_fputc(utf_file *stream, utf_c32 code)
{
    typedef void (*fputc_ptr)(utf_file *, utf_c32);
    static fputc_ptr fputc_table[] = {
        utf_8_fputc_,
        utf_8_fputc_,
        utf_16_fputc_,
        utf_16_fputc_,
        utf_16_fputc_,
        utf_32_fputc_,
        utf_32_fputc_,
        utf_32_fputc_
    };

    if (!utf_is_valid_code_point(code))
        return UTF_EOF;

    fputc_table[stream->encoding](stream, code);
    return code;
}

size_t utf_fread(utf_file *UTF_RESTRICT stream,
                 utf_c8 *UTF_RESTRICT buf,
                 size_t count)
{
    typedef utf_bool (*fread_next_ptr)(utf_file *, utf_c8 *);
    static fread_next_ptr fread_next_table[] = {
        utf_8_fread_next_,
        utf_8_fread_next_,
        utf_16_fread_next_,
        utf_16_fread_next_,
        utf_16_fread_next_,
        utf_32_fread_next_,
        utf_32_fread_next_,
        utf_32_fread_next_
    };

    if (!stream || !buf || count == 0)
        return 0;

    size_t n = 0;

    while (count-- > 0) {
        size_t len;

        if (!fread_next_table[stream->encoding](stream, buf))
            break;

        buf += utf_8_length_from_lead(*buf);
        n++;
    }

    *buf = 0;
    return n;
}

size_t utf_fwrite(utf_file *UTF_RESTRICT stream,
                  const utf_c8 *UTF_RESTRICT buf,
                  size_t count)
{
    typedef void (*fwrite_next_ptr)(utf_file *, const utf_c8 *);
    static fwrite_next_ptr fwrite_next_table[] = {
        utf_8_fwrite_next_,
        utf_8_fwrite_next_,
        utf_16_fwrite_next_,
        utf_16_fwrite_next_,
        utf_16_fwrite_next_,
        utf_32_fwrite_next_,
        utf_32_fwrite_next_,
        utf_32_fwrite_next_
    };

    if (!stream || !buf || count == 0)
        return 0;

    size_t n = 0;

    while (count-- > 0) {
        fwrite_next_table[stream->encoding](stream, buf);

        buf += utf_8_length_from_lead(*buf);
        n++;
    }

    return n;
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

static utf_bool utf_8_fread_bom_(utf_file *stream)
{
    if (getc(stream->file) == 0xEF &&
        getc(stream->file) == 0xBB &&
        getc(stream->file) == 0xBF)
        return utf_true;

    rewind(stream);
    return utf_false;
}

static utf_endianness utf_fread_bom_(utf_file *stream)
{
    utf_c8 bytes[4] = {0, 0, 0, 0};

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

static size_t utf_8_fread_next_raw_(utf_file *UTF_RESTRICT stream,
                                    utf_c8 *UTF_RESTRICT sym)
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

    if (len == 1) {
        stream->state = UTF_OK;
        return 1;
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

static size_t utf_16_fread_next_raw_(utf_file *UTF_RESTRICT stream,
                                     utf_c16 *UTF_RESTRICT sym)
{
    {
        int c;
        if ((c = getc(stream->file)) == EOF) {
            stream->state = UTF_OK;
            return 0;
        }
        ungetc(c, stream->file);
    }

    if (fread(&sym[0], 2, 1, stream->file) == 0) {
        stream->state = UTF_TRUNCATED;
        return 0;
    }

    if (utf_system_endianness() != stream->endianness)
        sym[0] = utf_16_byteswap(sym[0]);

    if (utf_is_trail_surrogate(sym[0])) {
        stream->state = UTF_INVALID_LEAD;
        return 0;
    }

    if (utf_is_lead_surrogate(sym[0])) {
        if (fread(&sym[1], 2, 1, stream->file) == 0) {
            stream->state = UTF_TRUNCATED;
            return 0;
        }

        if (utf_system_endianness() != stream->endianness)
            sym[1] = utf_16_byteswap(sym[1]);

        if (!utf_is_trail_surrogate(sym[1])) {
            stream->state = UTF_INVALID_TRAIL;
            return 0;
        }

        stream->state = UTF_OK;
        return 2;
    }

    stream->state = UTF_OK;
    return 1;
}

static size_t utf_32_fread_next_raw_(utf_file *UTF_RESTRICT stream,
                                     utf_c32 *UTF_RESTRICT sym)
{
    {
        int c;
        if ((c = getc(stream->file)) == EOF) {
            stream->state = UTF_OK;
            return 0;
        }
        ungetc(c, stream->file);
    }

    if (fread(sym, 4, 1, stream->file) == 0) {
        stream->state = UTF_TRUNCATED;
        return 0;
    }

    if (utf_system_endianness() != stream->endianness)
        *sym = utf_32_byteswap(*sym);

    if (!utf_is_valid_code_point(*sym)) {
        stream->state = UTF_INVALID_CODE_POINT;
        return 0;
    }

    stream->state = UTF_OK;
    return 1;
}

static utf_bool utf_8_fread_next_(utf_file *UTF_RESTRICT stream,
                                  utf_c8 *UTF_RESTRICT buf)
{
    utf_c8 sym[4];
    size_t len;

    if ((len = utf_8_fread_next_raw_(stream, sym)) == 0)
        return utf_false;

    memcpy(buf, sym, len * sizeof(utf_c8));
    return utf_true;
}

static utf_bool utf_16_fread_next_(utf_file *UTF_RESTRICT stream,
                                   utf_c8 *UTF_RESTRICT buf)
{
    utf_c16 sym[2];
    size_t len;

    if ((len = utf_16_fread_next_raw_(stream, sym)) == 0)
        return utf_false;

    utf_16_chr_to_8_(sym, buf);
    return utf_true;
}

static utf_bool utf_32_fread_next_(utf_file *UTF_RESTRICT stream,
                                   utf_c8 *UTF_RESTRICT buf)
{
    utf_c32 sym;
    size_t len;

    if ((len = utf_32_fread_next_raw_(stream, &sym)) == 0)
        return utf_false;

    utf_32_chr_to_8_(&sym, buf);
    return utf_true;
}

static utf_c32 utf_8_fgetc_(utf_file *stream)
{
    utf_c8 buf[4];
    size_t count;
    utf_c32 cp = 0;

    if ((count = utf_8_fread_next_raw_(stream, buf)) == 0)
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

static utf_c32 utf_16_fgetc_(utf_file *stream)
{
    utf_c16 buf[2];
    size_t count;
    utf_c32 cp = 0;

    if ((count = utf_16_fread_next_raw_(stream, buf)) == 0)
        return UTF_EOF;

    if (count == 1) {
        cp |= buf[0];
    } else if (count == 2) {
        cp |= buf[0] - 0xD800 << 10;
        cp |= buf[1] - 0xDC00;
        cp += 0x10000;
    }

    return cp;
}

static utf_c32 utf_32_fgetc_(utf_file *stream)
{
    utf_c32 cp;

    return utf_32_fread_next_raw_(stream, &cp) ? cp : UTF_EOF;
}

static void utf_8_fwrite_next_(utf_file *UTF_RESTRICT stream,
                               const utf_c8 *UTF_RESTRICT c)
{
    fwrite(c, 1, utf_8_length_from_lead(*c), stream->file);
}

static void utf_16_fwrite_next_(utf_file *UTF_RESTRICT stream,
                                const utf_c8 *UTF_RESTRICT c)
{
    utf_c16 sym[2];
    size_t len;

    utf_8_chr_to_16_(c, sym);
    len = !utf_is_surrogate(*sym) ? 1 : 2;

    if (stream->endianness != utf_system_endianness()) {
        sym[0] = utf_16_byteswap(sym[0]);
        if (len == 2)
            sym[1] = utf_16_byteswap(sym[1]);
    }

    fwrite(sym, 2, len, stream->file);
}

static void utf_32_fwrite_next_(utf_file *UTF_RESTRICT stream,
                                const utf_c8 *UTF_RESTRICT c)
{
    utf_c32 sym;

    utf_8_chr_to_32_(c, &sym);

    if (stream->endianness != utf_system_endianness())
        sym = utf_32_byteswap(sym);

    fwrite(&sym, 4, 1, stream->file);
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

static void utf_16_fputc_(utf_file *stream, utf_c32 code)
{
    if (code < 0x10000) {
        if (stream->endianness == UTF_BIG_ENDIAN) {
            putc(code >> 8, stream->file);
            putc(code     , stream->file);
        } else {
            putc(code     , stream->file);
            putc(code >> 8, stream->file);
        }
    } else if (code < 0x110000) {
        utf_c32 tmp = code - 0x10000;
        utf_c16 high = (tmp >> 10)   + 0xD800;
        utf_c16 low  = (tmp & 0x3FF) + 0xDC00;

        if (stream->endianness == UTF_BIG_ENDIAN) {
            putc(high >> 8, stream->file);
            putc(high     , stream->file);
            putc(low  >> 8, stream->file);
            putc(low      , stream->file);
        } else {
            putc(low      , stream->file);
            putc(low  >> 8, stream->file);
            putc(high     , stream->file);
            putc(high >> 8, stream->file);
        }
    }
}

static void utf_32_fputc_(utf_file *stream, utf_c32 code)
{
    if (stream->endianness == UTF_BIG_ENDIAN) {
        putc(code >> 24, stream->file);
        putc(code >> 16, stream->file);
        putc(code >>  8, stream->file);
        putc(code      , stream->file);
    } else {
        putc(code      , stream->file);
        putc(code >>  8, stream->file);
        putc(code >> 16, stream->file);
        putc(code >> 24, stream->file);
    }
}
