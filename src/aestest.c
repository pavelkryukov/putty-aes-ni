/**
 * aestest.c
 *
 * Unit testing for AES cryptoalgorithm
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

static void test(KeyType keytype, TestType testtype, unsigned int seed, unsigned blocklen, unsigned blockcount, FILE *file, unsigned char* arena)
{
    unsigned data_size = blocklen * blockcount;
    void *handle = aes_make_context();
    const size_t keylen = (size_t)keytype;
    const size_t usedlen = 2 * keylen + data_size;
    unsigned char * const key = arena;
    unsigned char * const blk = arena + keylen;
    unsigned char * const iv  = arena + keylen + data_size;

    unsigned i;

    srand(seed);

    for (i = 0; i < usedlen; ++i)
        arena[i] = rand();

    switch (keytype)
    {
    case AES128:
        aes128_key(handle, key);
        break;
    case AES192:
        aes192_key(handle, key);
        break;
    case AES256:
        aes256_key(handle, key);
        break;
    }

    aes_iv(handle, iv);

    unsigned char * blk_start = blk;
    for (i = 0; i < blockcount; ++i)
    {
        switch (testtype)
        {
        case ENCRYPT:
            aes_ssh2_encrypt_blk(handle, blk_start, blocklen);
            break;
        case DECRYPT:
            aes_ssh2_decrypt_blk(handle, blk_start, blocklen);
            break;
        case SDCTR:
            aes_ssh2_sdctr(handle, blk_start, blocklen);
            break;
        }
        blk_start += blocklen;
    }

    for (i = 0; i < data_size; ++i)
        fprintf(file,"%02x ", blk[i]);

    fprintf(file, "\n\n");

    aes_free_context(handle);
}

int main(int args, char** argv)
{
    unsigned char* arena;
    FILE *fp = fopen("test-output.txt", "w");
    KeyType keytypes[] = {AES128, AES192, AES256};
    size_t keytypes_s = DIM(keytypes);

    size_t blocksizes[] = {64, 128, 192, 256, 1024, 2048, 65536};    
    size_t blocksizes_s = DIM(blocksizes);

    size_t blockcounts[] = {1, 2, 4, 16};
    size_t blockcounts_s = DIM(blockcounts);

    unsigned k, b, seed, n, c;
    
    n = strtoul(argv[1], NULL, 0);
    arena = (unsigned char*)malloc(1 << 20);

    for (seed = 2; seed < n; ++seed)
        for (b = 0; b < blocksizes_s; ++b)
            for (k = 0; k < keytypes_s; ++k)
                 for (c = 0; c < blockcounts_s; ++c) {
                    test(keytypes[k], ENCRYPT, seed, blocksizes[b], blockcounts[c], fp, arena);
                    test(keytypes[k], DECRYPT, seed, blocksizes[b], blockcounts[c], fp, arena);
                    test(keytypes[k], SDCTR  , seed, blocksizes[b], blockcounts[c], fp, arena);
                 }

    free(arena);
    fclose(fp);
    
    return 0;
}
