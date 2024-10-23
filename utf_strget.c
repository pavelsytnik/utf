#include "utf_strget.h"

#include "utf_chrconv.h"

utf_c32 utf_8_strget(const utf_c8 *s, size_t i)
{
    utf_c32 cp;
    utf_8_chr_to_32_(utf_8_strat(s, i), &cp);
    return cp;
}

utf_c8 *utf_8_strat(const utf_c8 *s, size_t i)
{
    while (i-- > 0)
        s += utf_8_length_from_lead(*s);
    return s;
}
