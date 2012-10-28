/**
 * puttymem.c
 *
 * Simple implementation of some Putty memory functions
 * for easier test building
 *
 * @author kryukov@frtk.ru
 * @version 1.0
 *
 * For Putty AES NI project
 * http://putty-aes-ni.googlecode.com/
 */

#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <malloc.h>

#ifdef __MINGW32__
#define _mm_malloc(a, b) __mingw_aligned_malloc(a, b)
#define _mm_free(a)      __mingw_aligned_free(a)
#endif

void *safemalloc(size_t n, size_t size)
{
    void *p;
    if (n > INT_MAX / size) {
        p = NULL;
    }
    else {
        size *= n;
        if (size == 0)
            size = 1;
        p = _mm_malloc(size, 16);
    }

    return p;
}

void safefree(void *ptr)
{
    if (ptr)
        _mm_free(ptr);
}

void smemclr(void *b, size_t n) {
    volatile char *vp;

    if (b && n > 0) {
        memset(b, 0, n);
        vp = b;
        while (*vp) vp++;
    }
}
