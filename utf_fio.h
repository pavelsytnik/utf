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
    UTF_U8,
    UTF_U8BOM,
    UTF_U16,
    UTF_U16LE,
    UTF_U16BE,
    UTF_U32,
    UTF_U32LE,
    UTF_U32BE
};

enum utf_file_mode {
    UTF_READ   = 0x01,
    UTF_WRITE  = 0x02,
    UTF_APPEND = 0x04
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

uint32_t utf_u8getc_s(FILE *stream, enum utf_error *err);
int utf_u8getc(utf_c8 *bytes, FILE *stream);

#endif /* UTF_FIO_H */
