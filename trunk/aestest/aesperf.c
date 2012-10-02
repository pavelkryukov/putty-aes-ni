/**
 * aesperf.c
 *
 * Measuring time of AES cryptoalgorithm
 *
 * @author kryukov@frtk.ru
 * @version 1.0
 *
 * For Putty AES NI project
 * http://putty-aes-ni.googlecode.com/
 */

#include <stdio.h>
#include <stdlib.h>

#include "ssh.h"

typedef enum
{
    AES128 = 128,
    AES192 = 192,
    AES256 = 256
} KeyType;

typedef enum
{
    ENCRYPT,
    DECRYPT,
    SDCTR
} TestType;

static void test(KeyType keytype, TestType testtype, unsigned int seed, unsigned blocklen, FILE *file)
{
    void *handle = aes_make_context();
    const size_t keylen = (size_t)keytype;
    unsigned char *key = (unsigned char*)malloc(sizeof(unsigned char) * keylen);
    unsigned char *blk = (unsigned char*)malloc(sizeof(unsigned char) * blocklen);
    unsigned char *iv  = (unsigned char*)malloc(sizeof(unsigned char) * keylen);
    clock_t now;

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

    now = clock();
    switch (testtype)
    {
    case ENCRYPT:
        aes_ssh2_encrypt_blk(handle, blk, blocklen);
        break;
    case DECRYPT:
        aes_ssh2_decrypt_blk(handle, blk, blocklen);
        break;
    case SDCTR:
        aes_ssh2_sdctr(handle, blk, blocklen);
        break;
    }
    
    now = clock() - now;
    fprintf(file, "%d\t%d\t%d\t%d\n", testtype, keytype, blocklen, now);

    aes_free_context(handle);
    free(key), free(blk), free(iv);
}

#define DIM(A) (sizeof(A) / sizeof(A[0]))

int main()
{
    FILE *fp = fopen("perf-output.txt", "w");
    KeyType keytypes[] = {AES128, AES192, AES256};
    size_t keytypes_s = DIM(keytypes);
    int b, k;
    
    for (b = (1 << 20); b < (1 << 30); b += ((b >> 1) & ~0xf))
        for (k = 0; k < keytypes_s; ++k) {
            test(keytypes[k], ENCRYPT, 4134, b, fp);
            test(keytypes[k], DECRYPT, 2343, b, fp);
            test(keytypes[k], SDCTR, 4321, b, fp);
            fflush(fp);
        }
    
    fclose(fp);
    
    return 0;
}
