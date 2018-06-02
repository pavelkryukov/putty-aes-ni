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

#include <misc.h>

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
        p = malloc(size);
    }

    return p;
}

void safefree(void *ptr)
{
    if (ptr)
        free(ptr);
}

void smemclr(void *b, size_t n) {
    volatile char *vp;

    if (b && n > 0) {
        memset(b, 0, n);
        vp = b;
        while (*vp) vp++;
    }
}

int smemeq(const void *av, const void *bv, size_t len)
{
    const unsigned char *a = (const unsigned char *)av;
    const unsigned char *b = (const unsigned char *)bv;
    unsigned val = 0;

    while (len-- > 0) {
        val |= *a++ ^ *b++;
    }
    /* Now val is 0 iff we want to return 1, and in the range
     * 0x01..0xFF iff we want to return 0. So subtracting from 0x100
     * will clear bit 8 iff we want to return 0, and leave it set iff
     * we want to return 1, so then we can just shift down. */
    return (0x100 - val) >> 8;
}

struct strbuf_impl {
    int size;
    struct strbuf visible;
};

void strbuf_free(strbuf *buf_o)
{
    struct strbuf_impl *buf = (struct strbuf_impl *)(((char *)(buf_o)) - offsetof(struct strbuf_impl, visible));
    if (buf->visible.s) {
        smemclr(buf->visible.s, buf->size);
        sfree(buf->visible.s);
    }
    sfree(buf);
}

ptrlen make_ptrlen(const void *ptr, size_t len)
{
    ptrlen pl;
    pl.ptr = ptr;
    pl.len = len;
    return pl;
}
