#ifndef utf_fio_h_
#define utf_fio_h_

#include "utf_bool.h"
#include "utf_char.h"
#include "utf_error.h"
#include "utf_lang.h"

#include <limits.h>
#include <stddef.h>
#include <stdio.h>

#define UTF_EOF UINT_MAX

typedef struct utf_file          utf_file;
typedef enum   utf_file_mode     utf_file_mode;
typedef enum   utf_file_encoding utf_file_encoding;

enum utf_file_mode {
    UTF_READ   = 0x01,
    UTF_WRITE  = 0x02,
    UTF_APPEND = 0x04
};

enum utf_file_encoding {
    UTF_8,
    UTF_8_SIG,
    UTF_16,
    UTF_16_BE,
    UTF_16_LE,
    UTF_32,
    UTF_32_BE,
    UTF_32_LE
};

utf_file *utf_fopen(const char *filename,
                    utf_file_mode mode,
                    utf_file_encoding encoding);
utf_bool utf_fclose(utf_file *stream);

FILE *utf_c_file(const utf_file *stream);
utf_error utf_ferror(const utf_file *stream);
utf_bool utf_feof(const utf_file *stream);

utf_c32 utf_fgetc(utf_file *stream);
utf_c32 utf_fputc(utf_file *stream, utf_c32 code);

size_t utf_fread(utf_file *UTF_RESTRICT stream,
                 utf_c8 *UTF_RESTRICT buf,
                 size_t count);
size_t utf_fwrite(utf_file *UTF_RESTRICT stream,
                  const utf_c8 *UTF_RESTRICT buf,
                  size_t count);

#endif /* !defined(utf_fio_h_) */
