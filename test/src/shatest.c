/**
 * shatest.c
 *
 * Unit testing for SHA1 cryptoalgorithm
 *
 * @author kryukov@frtk.ru
 * @version 2.0
 *
 * For Putty AES NI project
 * http://putty-aes-ni.googlecode.com/
 */

#include <stdio.h>
#include <stdlib.h>

#include "coverage.h"
#include "defines.h"

static void test(HashType hashtype, unsigned int seed, unsigned blocklen, FILE *file, unsigned char* arena)
{
    unsigned int i;
    unsigned char * const data   = arena;
    unsigned char * const result = arena + blocklen;
    srand(seed);

    for (i = 0; i < blocklen; ++i)
        arena[i] = rand();

    unsigned int reslen = 0;

    switch (hashtype)
    {
    case SHA1:
        SHA_Simple(data, blocklen, result);
        reslen = 5;
        break;
    case SHA256:
        SHA256_Simple(data, blocklen, result);
        reslen = 8;
        break;
    }

    for (i = 0; i < reslen; ++i)
        fprintf(file,"%02x ", result[i]);

    fprintf(file, "\n\n");
}

int main(int args, char** argv)
{
    unsigned char* arena;
    FILE *fp = fopen("test-output.txt", "w");
    HashType hashtypes[] = {SHA1, SHA256};
    size_t hashtypes_s = DIM(hashtypes);

    size_t blocksizes[] = {32, 64, 100, 101, 128, 192, 256, 1024, 2048, 65536};    
    size_t blocksizes_s = DIM(blocksizes);
    unsigned k, b, seed, n;

    n = strtoul(argv[1], NULL, 0);
    arena = (unsigned char*)malloc(1 << 20);

    for (seed = 2; seed < n; ++seed)
        for (b = 0; b < blocksizes_s; ++b)
            for (k = 0; k < hashtypes_s; ++k)
                test(hashtypes[k], seed, blocksizes[b], fp, arena);

    free(arena);
    fclose(fp);
    
    return 0;
}
