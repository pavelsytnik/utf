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
    UTF_APPEND = 0x04,
    UTF_BINARY = 0x08
};

struct utf_file *utf_fopen(const char *filename,
                           enum utf_file_mode mode,
                           enum utf_file_encoding encoding);

bool utf_fclose(struct utf_file *stream);

FILE *utf_c_file(const struct utf_file *stream);

char8_t *utf_u8fread(char8_t *buf, size_t count, FILE *stream);
int utf_u8getc(char8_t *bytes, FILE *stream);

uint32_t utf_u8getc_s(FILE *stream, enum utf_error *err);
size_t utf_u8fread_s(char8_t *buf,
                     size_t count,
                     FILE *stream,
                     enum utf_error *err);

bool utf_u8fread_bom(FILE *stream);

#endif /* UTF_FIO_H */
