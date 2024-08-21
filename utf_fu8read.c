#include "utf.h"

char8_t *utf_fu8read(char8_t *buf, size_t count, FILE *stream)
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
