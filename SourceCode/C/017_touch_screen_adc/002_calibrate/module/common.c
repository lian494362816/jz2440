#include "common.h"

void * memset(void * s, int c, int count)
{
    unsigned long *sl = (unsigned long *) s;
    char *s8;

    unsigned long cl = 0;
    int i;

    /* do it one word at a time (32 bits or 64 bits) while possible */
    if ( ((unsigned long)s & (sizeof(*sl) - 1)) == 0) {
        for (i = 0; i < sizeof(*sl); i++) {
            cl <<= 8;
            cl |= c & 0xff;
        }
        while (count >= sizeof(*sl)) {
            *sl++ = cl;
            count -= sizeof(*sl);
        }
    }

    s8 = (char *)sl;
    while (count--)
        *s8++ = c;

    return s;
}

int strlen(const char * s)
{
    const char *sc;

    for (sc = s; *sc != '\0'; ++sc)
        /* nothing */;
    return sc - s;
}

int strcmp(const char * cs,const char * ct)
{
    register signed char __res;

    while (1) {
        if ((__res = *cs - *ct++) != 0 || !*cs++)
            break;
    }

    return __res;
}

void swap_int(int *a, int *b)
{
    int tmp = 0;

    tmp = *a;
    *a = *b;
    *b = tmp;
}
