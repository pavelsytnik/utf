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
    utf_bool bom_aware;
};

static const char *utf_fmode_str_(utf_file_mode mode);
static void utf_file_decompose_encoding_(utf_file *stream,
                                         utf_file_encoding enc);
static void utf_file_process_bom_(utf_file *stream, utf_file_mode mode);
static utf_bool utf_file_try_swap_endian_(utf_file *stream);

static void utf_code_point_copy_(const utf_c32 *UTF_RESTRICT src,
                                       utf_c32 *UTF_RESTRICT dst);
static void utf_8_chr_copy_(const utf_c8 *UTF_RESTRICT src,
                                  utf_c8 *UTF_RESTRICT dst);

static size_t utf_8_fread_next_(utf_file *UTF_RESTRICT stream,
                                  utf_c8 *UTF_RESTRICT c);
static size_t utf_16_be_fread_next_(utf_file *UTF_RESTRICT stream,
                                    utf_c16 *UTF_RESTRICT c);
static size_t utf_16_le_fread_next_(utf_file *UTF_RESTRICT stream,
                                    utf_c16 *UTF_RESTRICT c);
static size_t utf_32_be_fread_next_(utf_file *UTF_RESTRICT stream,
                                    utf_c32 *UTF_RESTRICT c);
static size_t utf_32_le_fread_next_(utf_file *UTF_RESTRICT stream,
                                    utf_c32 *UTF_RESTRICT c);

static void utf_8_fwrite_next_(utf_file *UTF_RESTRICT stream,
                               const utf_c8 *UTF_RESTRICT c);
static void utf_16_be_fwrite_next_(utf_file *UTF_RESTRICT stream,
                                   const utf_c8 *UTF_RESTRICT c);
static void utf_16_le_fwrite_next_(utf_file *UTF_RESTRICT stream,
                                   const utf_c8 *UTF_RESTRICT c);
static void utf_32_be_fwrite_next_(utf_file *UTF_RESTRICT stream,
                                   const utf_c8 *UTF_RESTRICT c);
static void utf_32_le_fwrite_next_(utf_file *UTF_RESTRICT stream,
                                   const utf_c8 *UTF_RESTRICT c);

static void utf_8_fputc_(utf_file *stream, utf_c32 code);
static void utf_16_be_fputc_(utf_file *stream, utf_c32 code);
static void utf_16_le_fputc_(utf_file *stream, utf_c32 code);
static void utf_32_be_fputc_(utf_file *stream, utf_c32 code);
static void utf_32_le_fputc_(utf_file *stream, utf_c32 code);

utf_file *utf_fopen(const char *filename,
                    utf_file_mode mode,
                    utf_file_encoding encoding)
{
    utf_file *file;
    FILE *c_file;

    file = malloc(sizeof(utf_file));
    if (!file) return NULL;

    c_file = fopen(filename, utf_fmode_str_(mode));
    if (!c_file) { free(file); return NULL; }

    file->file = c_file;
    file->state = UTF_OK;
    utf_file_decompose_encoding_(file, encoding);

    utf_file_process_bom_(file, mode);

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
    typedef utf_bool (*fread_next_ptr)(utf_file *, void *);
    static fread_next_ptr fread_next_table[] = {
        utf_8_fread_next_,
        NULL,
        NULL,
        utf_16_be_fread_next_,
        utf_16_le_fread_next_,
        NULL,
        utf_32_be_fread_next_,
        utf_32_le_fread_next_
    };

    typedef void (*chr_to_code_point_ptr)(const void *, utf_c32 *);
    static chr_to_code_point_ptr chr_to_code_point_table[] = {
    #ifdef UTF_BIG_ENDIAN
        utf_8_chr_to_32_be,
        NULL,
        NULL,
        utf_16_be_chr_to_32_be,
        utf_16_le_chr_to_32_be,
        NULL,
        utf_code_point_copy_,
        utf_32_chr_to_opposite
    #else
        utf_8_chr_to_32_le,
        NULL,
        NULL,
        utf_16_be_chr_to_32_le,
        utf_16_le_chr_to_32_le,
        NULL,
        utf_32_chr_to_opposite,
        utf_code_point_copy_
    #endif
    };

    utf_c8 bytes[4];
    utf_c32 cp;

    if (!fread_next_table[stream->encoding](stream, bytes))
        return UTF_EOF;

    chr_to_code_point_table[stream->encoding](bytes, &cp);
    return cp;
}

utf_c32 utf_fputc(utf_file *stream, utf_c32 code)
{
    typedef void (*fputc_ptr)(utf_file *, utf_c32);
    static fputc_ptr fputc_table[] = {
        utf_8_fputc_,
        NULL,
        NULL,
        utf_16_be_fputc_,
        utf_16_le_fputc_,
        NULL,
        utf_32_be_fputc_,
        utf_32_le_fputc_
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
    typedef utf_bool (*fread_next_ptr)(utf_file *, void *);
    static fread_next_ptr fread_next_table[] = {
        utf_8_fread_next_,
        NULL,
        NULL,
        utf_16_be_fread_next_,
        utf_16_le_fread_next_,
        NULL,
        utf_32_be_fread_next_,
        utf_32_le_fread_next_
    };

    typedef void (*chr_to_8_ptr)(const void *, utf_c8 *);
    static chr_to_8_ptr chr_to_8_table[] = {
        utf_8_chr_copy_,
        NULL,
        NULL,
        utf_16_be_chr_to_8,
        utf_16_le_chr_to_8,
        NULL,
        utf_32_be_chr_to_8,
        utf_32_le_chr_to_8
    };

    size_t n = 0;
    utf_c8 bytes[4];

    if (!stream || !buf || count == 0)
        return 0;

    while (count-- > 0) {
        if (!fread_next_table[stream->encoding](stream, bytes))
            break;

        chr_to_8_table[stream->encoding](bytes, buf);
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
        NULL,
        NULL,
        utf_16_be_fwrite_next_,
        utf_16_le_fwrite_next_,
        NULL,
        utf_32_be_fwrite_next_,
        utf_32_le_fwrite_next_
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

static const char *utf_fmode_str_(utf_file_mode mode)
{
    switch (mode) {
    case UTF_READ:
        return "rb";
    case UTF_WRITE:
    case UTF_WRITE | UTF_TRUNC:
        return "wb";
    case UTF_APPEND:
    case UTF_WRITE | UTF_APPEND:
        return "ab";
    case UTF_READ | UTF_WRITE:
        return "r+b";
    case UTF_READ | UTF_WRITE | UTF_TRUNC:
        return "w+b";
    case UTF_READ | UTF_WRITE | UTF_APPEND:
        return "a+b";
    default:
        return NULL;
    }
}

static utf_bool utf_file_try_swap_endian_(utf_file *stream)
{
#ifdef UTF_BIG_ENDIAN
    if (stream->encoding == UTF_16_BE) {
        stream->encoding = UTF_16_LE;
        return utf_true;
    }
    if (stream->encoding == UTF_32_BE) {
        stream->encoding = UTF_32_LE;
        return utf_true;;
    }
#else
    if (stream->encoding == UTF_16_LE) {
        stream->encoding = UTF_16_BE;
        return utf_true;
    }
    if (stream->encoding == UTF_32_LE) {
        stream->encoding = UTF_32_BE;
        return utf_true;
    }
#endif

    return utf_false;
}

static void utf_file_decompose_encoding_(utf_file *stream,
                                         utf_file_encoding enc)
{
    stream->bom_aware = enc == UTF_8_SIG ||
                        enc == UTF_16    ||
                        enc == UTF_32;

    if (enc == UTF_8_SIG)
        stream->encoding = UTF_8;
#ifdef UTF_BIG_ENDIAN
    else if (enc == UTF_16)
        stream->encoding = UTF_16_BE;
    else if (enc == UTF_32)
        stream->encoding = UTF_32_BE;
#else
    else if (enc == UTF_16)
        stream->encoding = UTF_16_LE;
    else if (enc == UTF_32)
        stream->encoding = UTF_32_LE;
#endif
    else
        stream->encoding = enc;
}

static void utf_file_process_bom_(utf_file *stream, utf_file_mode mode)
{
    if (stream->bom_aware) {
        switch (mode) {
        case UTF_READ:
        case UTF_READ | UTF_WRITE:
        {
            utf_c32 cp = utf_fgetc(stream);

            if (cp == 0xFFFE && utf_file_try_swap_endian_(stream))
                return;

            if (cp == 0xFEFF)
                return;

            rewind(stream->file);

            break;
        }
        case UTF_WRITE:
        case UTF_WRITE | UTF_TRUNC:
        case UTF_WRITE | UTF_TRUNC | UTF_READ:
            utf_fputc(stream, 0xFEFF);
            break;
        }
    }
}

static void utf_code_point_copy_(const utf_c32 *UTF_RESTRICT src,
                                       utf_c32 *UTF_RESTRICT dst)
{
    memcpy(dst, src, 4);
}

static void utf_8_chr_copy_(const utf_c8 *UTF_RESTRICT src,
                                  utf_c8 *UTF_RESTRICT dst)
{
    memcpy(dst, src, utf_8_length_from_lead(*src));
}

static size_t utf_8_fread_next_(utf_file *UTF_RESTRICT stream,
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

static size_t utf_16_be_fread_next_(utf_file *UTF_RESTRICT stream,
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

#ifdef UTF_BIG_ENDIAN
    if (utf_is_trail_surrogate(sym[0])) {
#else
    if (utf_is_trail_surrogate(utf_c16_bswap(sym[0]))) {
#endif
        stream->state = UTF_INVALID_LEAD;
        return 0;
    }

#ifdef UTF_BIG_ENDIAN
    if (utf_is_lead_surrogate(sym[0])) {
#else
    if (utf_is_lead_surrogate(utf_c16_bswap(sym[0]))) {
#endif
        if (fread(&sym[1], 2, 1, stream->file) == 0) {
            stream->state = UTF_TRUNCATED;
            return 0;
        }

    #ifdef UTF_BIG_ENDIAN
        if (!utf_is_trail_surrogate(sym[1])) {
    #else
        if (!utf_is_trail_surrogate(utf_c16_bswap(sym[1]))) {
    #endif
            stream->state = UTF_INVALID_TRAIL;
            return 0;
        }

        stream->state = UTF_OK;
        return 2;
    }

    stream->state = UTF_OK;
    return 1;
}

static size_t utf_16_le_fread_next_(utf_file *UTF_RESTRICT stream,
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

#ifdef UTF_LITTLE_ENDIAN
    if (utf_is_trail_surrogate(sym[0])) {
#else
    if (utf_is_trail_surrogate(utf_c16_bswap(sym[0]))) {
#endif
        stream->state = UTF_INVALID_LEAD;
        return 0;
    }

#ifdef UTF_LITTLE_ENDIAN
    if (utf_is_lead_surrogate(sym[0])) {
#else
    if (utf_is_lead_surrogate(utf_c16_bswap(sym[0]))) {
#endif
        if (fread(&sym[1], 2, 1, stream->file) == 0) {
            stream->state = UTF_TRUNCATED;
            return 0;
        }

    #ifdef UTF_LITTLE_ENDIAN
        if (!utf_is_trail_surrogate(sym[1])) {
    #else
        if (!utf_is_trail_surrogate(utf_c16_bswap(sym[1]))) {
    #endif
            stream->state = UTF_INVALID_TRAIL;
            return 0;
        }

        stream->state = UTF_OK;
        return 2;
    }

    stream->state = UTF_OK;
    return 1;
}

static size_t utf_32_be_fread_next_(utf_file *UTF_RESTRICT stream,
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
    
#ifdef UTF_BIG_ENDIAN
    if (!utf_is_valid_code_point(*sym)) {
#else
    if (!utf_is_valid_code_point(utf_c32_bswap(*sym))) {
#endif
        stream->state = UTF_INVALID_CODE_POINT;
        return 0;
    }

    stream->state = UTF_OK;
    return 1;
}

static size_t utf_32_le_fread_next_(utf_file *UTF_RESTRICT stream,
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

#ifdef UTF_LITTLE_ENDIAN
    if (!utf_is_valid_code_point(*sym)) {
#else
    if (!utf_is_valid_code_point(utf_c32_bswap(*sym))) {
#endif
        stream->state = UTF_INVALID_CODE_POINT;
        return 0;
    }

    stream->state = UTF_OK;
    return 1;
}

static void utf_8_fwrite_next_(utf_file *UTF_RESTRICT stream,
                               const utf_c8 *UTF_RESTRICT c)
{
    fwrite(c, 1, utf_8_length_from_lead(*c), stream->file);
}

static void utf_16_be_fwrite_next_(utf_file *UTF_RESTRICT stream,
                                   const utf_c8 *UTF_RESTRICT c)
{
    utf_c16 sym[2];
    size_t len;

    utf_8_chr_to_16_be(c, sym);
#ifdef UTF_LITTLE_ENDIAN
    len = !utf_is_surrogate(utf_c16_bswap(*sym)) ? 1 : 2;
#else
    len = !utf_is_surrogate(*sym) ? 1 : 2;
#endif
    fwrite(sym, 2, len, stream->file);
}

static void utf_16_le_fwrite_next_(utf_file *UTF_RESTRICT stream,
                                   const utf_c8 *UTF_RESTRICT c)
{
    utf_c16 sym[2];
    size_t len;

    utf_8_chr_to_16_le(c, sym);
#ifdef UTF_BIG_ENDIAN
    len = !utf_is_surrogate(utf_c16_bswap(*sym)) ? 1 : 2;
#else
    len = !utf_is_surrogate(*sym) ? 1 : 2;
#endif
    fwrite(sym, 2, len, stream->file);
}

static void utf_32_be_fwrite_next_(utf_file *UTF_RESTRICT stream,
                                   const utf_c8 *UTF_RESTRICT c)
{
    utf_c32 sym;
    utf_8_chr_to_32_be(c, &sym);
    fwrite(&sym, 4, 1, stream->file);
}

static void utf_32_le_fwrite_next_(utf_file *UTF_RESTRICT stream,
                                   const utf_c8 *UTF_RESTRICT c)
{
    utf_c32 sym;
    utf_8_chr_to_32_le(c, &sym);
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

static void utf_16_be_fputc_(utf_file *stream, utf_c32 code)
{
    if (code < 0x10000) {
        putc(code >> 8, stream->file);
        putc(code     , stream->file);
    } else if (code < 0x110000) {
        utf_c32 tmp = code - 0x10000;
        utf_c16 high = (tmp >> 10)   + 0xD800;
        utf_c16 low  = (tmp & 0x3FF) + 0xDC00;

        putc(high >> 8, stream->file);
        putc(high     , stream->file);
        putc(low  >> 8, stream->file);
        putc(low      , stream->file);
    }
}

static void utf_16_le_fputc_(utf_file *stream, utf_c32 code)
{
    if (code < 0x10000) {
        putc(code     , stream->file);
        putc(code >> 8, stream->file);
    } else if (code < 0x110000) {
        utf_c32 tmp = code - 0x10000;
        utf_c16 high = (tmp >> 10)   + 0xD800;
        utf_c16 low = (tmp & 0x3FF) + 0xDC00;

        putc(low      , stream->file);
        putc(low  >> 8, stream->file);
        putc(high     , stream->file);
        putc(high >> 8, stream->file);
    }
}

static void utf_32_be_fputc_(utf_file *stream, utf_c32 code)
{
    putc(code >> 24, stream->file);
    putc(code >> 16, stream->file);
    putc(code >>  8, stream->file);
    putc(code      , stream->file);
}

static void utf_32_le_fputc_(utf_file *stream, utf_c32 code)
{
    putc(code      , stream->file);
    putc(code >>  8, stream->file);
    putc(code >> 16, stream->file);
    putc(code >> 24, stream->file);
}
