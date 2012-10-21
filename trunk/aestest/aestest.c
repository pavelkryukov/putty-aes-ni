/**
 * aestest.c
 *
 * Unit testing for AES cryptoalgorithm
 *
 * @author kryukov@frtk.ru
 * @version 1.5
 *
 * For Putty AES NI project
 * http://putty-aes-ni.googlecode.com/
 */

/*
 * Coverage:
 *
 * void *aes_make_context(void);
 * void aes_free_context(void *handle);
 * void aes128_key(void *handle, unsigned char *key);
 * void aes192_key(void *handle, unsigned char *key);
 * void aes256_key(void *handle, unsigned char *key);
 * void aes_iv(void *handle, unsigned char *iv);
 * void aes_ssh2_encrypt_blk(void *handle, unsigned char *blk, int len);
 * void aes_ssh2_decrypt_blk(void *handle, unsigned char *blk, int len);
*/

#include <stdio.h>
#include <stdlib.h>

#include "coverage.h"
#include "defines.h"

static void test(KeyType keytype, TestType testtype, unsigned int seed, unsigned blocklen, FILE *file)
{
    void *handle = aes_make_context();
    const size_t keylen = (size_t)keytype;
    unsigned char *key = (unsigned char*)malloc(sizeof(unsigned char) * keylen);
    unsigned char *blk = (unsigned char*)malloc(sizeof(unsigned char) * blocklen);
    unsigned char *iv  = (unsigned char*)malloc(sizeof(unsigned char) * keylen);

    unsigned i;

    srand(seed);

    for (i = 0; i < blocklen; ++i)
        blk[i] = rand();

    for (i = 0; i < keylen; ++i)
        key[i] = rand();

    for (i = 0; i < keylen; ++i)
        iv[i] = rand();

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

    aes_iv(handle, key);

    switch (testtype)
    {
    case ENCRYPT:
        aes_ssh2_encrypt_blk(handle, blk, blocklen);
        break;
    case DECRYPT:
        aes_ssh2_decrypt_blk(handle, blk, blocklen);
        break;
    case SDCTR:
        break;
    }
    
    for (i = 0; i < blocklen; ++i)
        fprintf(file,"%02x ", blk[i]);
        
    fprintf(file, "\n\n");
    
    aes_free_context(handle);
    free(key), free(blk), free(iv);
}

int main(int args, char** argv)
{
    FILE *fp = fopen("test-output.txt", "w");
    KeyType keytypes[] = {AES128, AES192, AES256};
    size_t keytypes_s = DIM(keytypes);

    size_t blocksizes[] = {64, 128, 192, 256, 1024, 2048, 65536};    
    size_t blocksizes_s = DIM(blocksizes);
    unsigned k, b, seed, n;

    n = strtoul(argv[1], NULL, 0);

    for (seed = 2; seed < n; ++seed)
        for (b = 0; b < blocksizes_s; ++b)
            for (k = 0; k < keytypes_s; ++k) {
                test(keytypes[k], ENCRYPT, seed, blocksizes[b], fp);
                test(keytypes[k], DECRYPT, seed, blocksizes[b], fp);
            }
    
    fclose(fp);
    
    return 0;
}
