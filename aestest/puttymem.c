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

void * aligned_malloc(size_t size, int align) {
	void *ptr, *mem;
	if (align < 0) {
		return NULL;
	}

    mem = malloc(size + align - 1 + sizeof(void*));
	
	if (mem != NULL) {
		ptr = (void*) (((unsigned int)mem + sizeof(void*) + align -1) & ~(align-1));
		*((void**)((unsigned int)ptr - sizeof(void*))) = mem;
		/* Return the address of aligned memory */
		return ptr; 
	}
	return NULL;
}

void aligned_free(void *p) {
	/* Get the address of the memory, stored at the
	 * start of our total memory area. Alternatively,
	 * you can use void *ptr = *((void **)p-1) instead 
	 * of the one below.
	 */
	void *ptr = *((void**)((unsigned int)p - sizeof(void*)));
	free(ptr);
	return;
}

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
        p = aligned_malloc(size, 16);
    }

    return p;
}

void safefree(void *ptr)
{
    if (ptr)
        aligned_free(ptr);
}

void smemclr(void *b, size_t n) {
    volatile char *vp;

    if (b && n > 0) {
        memset(b, 0, n);
        vp = b;
        while (*vp) vp++;
    }
}
