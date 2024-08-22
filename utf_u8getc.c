#include "utf.h"

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
