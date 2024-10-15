#ifndef UTF_FIO_H
#define UTF_FIO_H

#include "utf_char.h"
#include "utf_error.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define UTF_EOF 0xFFFFFFFFu

struct utf_file;

enum utf_file_encoding {
    UTF_FILE_ENCODING_UNKNOWN = 0,
    UTF_U8,
    UTF_U8_SIG,
    UTF_U16,
    UTF_U16_LE,
    UTF_U16_BE,
    UTF_U32,
    UTF_U32_LE,
    UTF_U32_BE
};

enum utf_file_mode {
    UTF_FILE_MODE_UNKNOWN = 0x00,
    UTF_READ              = 0x01,
    UTF_WRITE             = 0x02,
    UTF_APPEND            = 0x04
};

struct utf_file *utf_fopen(const char *filename,
                           enum utf_file_mode mode,
                           enum utf_file_encoding encoding);

bool utf_fclose(struct utf_file *stream);

FILE *utf_c_file(const struct utf_file *stream);
enum utf_error utf_ferror(const struct utf_file *stream);
bool utf_eof(const struct utf_file *stream);

size_t utf_u8fread(utf_c8 *restrict buf,
                   size_t count,
                   struct utf_file *restrict stream);
size_t utf_u16fread(utf_c16 *restrict buf,
                    size_t count,
                    struct utf_file *restrict stream);
size_t utf_u32fread(utf_c32 *restrict buf,
                    size_t count,
                    struct utf_file *restrict stream);

utf_c32 utf_fgetc8(struct utf_file *stream);
//int utf_u8getc(utf_c8 *bytes, FILE *stream);

#endif /* UTF_FIO_H */
