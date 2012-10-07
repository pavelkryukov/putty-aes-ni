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

#ifdef __GNUC__
static unsigned long long __rdtsc()
{
    unsigned hi, lo;
    __asm__ __volatile__
    (
        "rdtsc"
        : "=a"(lo)
        , "=d"(hi)
    );
    return ((unsigned long long)lo) | (((unsigned long long)hi)<<32);
}
#else
#include <intrin.h>
#endif

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

static void test(KeyType keytype, TestType testtype, unsigned blocklen, FILE *file, unsigned char* ptr)
{
    void *handle = aes_make_context();
    const size_t keylen = (size_t)keytype;
	unsigned char* const key = ptr + blocklen;
	unsigned char* const blk = ptr;
	unsigned char* const iv = ptr + keylen + blocklen;
    volatile unsigned long long now;

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

    now = __rdtsc();
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

    now = __rdtsc() - now;
    fprintf(file, "%d\t%d\t%d\t%llu\n", testtype, keytype, blocklen, now);

    aes_free_context(handle);
}

#define DIM(A) (sizeof(A) / sizeof(A[0]))
#define MAXBLK (1 << 25)
#define MEM (MAXBLK + 2 * 256)

int main()
{
    FILE *fp = fopen("perf-output.txt", "w");
    KeyType keytypes[] = {AES128, AES192, AES256};
    size_t keytypes_s = DIM(keytypes);
    int b, k, i;
	
	unsigned char* ptr = (unsigned char*)malloc(sizeof(unsigned char) * MEM);

    for (i = 0; i < MEM; ++i)
        ptr[i] = rand();

    for (b = 16; b <= MAXBLK; b <<= 1)
        for (i = 0; i < 30; ++i)
            for (k = 0; k < keytypes_s; ++k) {
                test(keytypes[k], ENCRYPT, b, fp, ptr);
                test(keytypes[k], DECRYPT, b, fp, ptr);
         /*     test(keytypes[k], SDCTR, 4321, b, fp);*/
                fflush(fp);
            }

	free(ptr);
    fclose(fp);

    return 0;
}
