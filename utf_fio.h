#ifndef UTF_FIO_H
#define UTF_FIO_H

#include "utf_char.h"
#include "utf_error.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define UTF_EOF 0xFFFFFFFFu

char8_t *utf_u8fread(char8_t *buf, size_t count, FILE *stream);
int utf_u8getc(char8_t *bytes, FILE *stream);

uint32_t utf_u8getc_s(FILE *stream, enum utf_error *err);
size_t utf_u8fread_s(char8_t *buf,
                     size_t count,
                     FILE *stream,
                     enum utf_error *err);

bool utf_u8fread_bom(FILE *stream);

#endif /* UTF_FIO_H */
