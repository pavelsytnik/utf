#include "utf.h"

char8_t *utf_u8fread(char8_t *buf, size_t count, FILE *stream)
{
    if (count == 0) return NULL;

    while (count > 1) {
        int ch = getc(stream);
        if (ch == EOF)
            break;

        int seqlen;
        if ((ch & 0x80) == 0x00)
            seqlen = 1;
        else if ((ch & 0xE0) == 0xC0)
            seqlen = 2;
        else if ((ch & 0xF0) == 0xE0)
            seqlen = 3;
        else if ((ch & 0xF8) == 0xF0)
            seqlen = 4;
        else
            break;

        if (seqlen > count - 1)
            break;

        *buf++ = ch;
        count--;

        for (int i = 1; i < seqlen; ++i) {
            ch = getc(stream);
            if (ch == EOF || (ch & 0xC0) != 0x80) {
                buf -= i;
                goto end;
            }
            *buf++ = ch;
            count--;
        }
    }
    end:
    *buf = 0;
    return buf;
}

int utf_u8getc(char8_t *bytes, FILE *stream)
{
    int c, contbytes, nbytes = 0;

    if ((c = getc(stream)) == EOF)
        return 0;

    bytes[0] = (char8_t) c;
    nbytes++;

    if ((c & 0x80) == 0x00)
        contbytes = 0;
    else if ((c & 0xE0) == 0xC0)
        contbytes = 1;
    else if ((c & 0xF0) == 0xE0)
        contbytes = 2;
    else if ((c & 0xF8) == 0xF0)
        contbytes = 3;
    else
        return -1;

    while (contbytes--) {
        if ((c = getc(stream)) == EOF)
            return -nbytes;
        bytes[nbytes++] = (char8_t) c;
        if ((c & 0xC0) != 0x80)
            return -nbytes;
    }

    return nbytes;
}
