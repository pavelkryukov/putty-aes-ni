/**
 * aesperf.c
 *
 * Measuring time of AES cryptoalgorithm
 *
 * @author kryukov@frtk.ru
 * @version 2.1
 *
 * For Putty AES NI project
 * http://putty-aes-ni.googlecode.com/
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "ssh.h"

typedef enum
{
    AES128 = 16,
    AES192 = 24,
    AES256 = 32
} KeyType;

typedef enum
{
    ENCRYPT,
    DECRYPT,
    SDCTR
} TestType;

typedef enum
{
    SHA1,
    SHA256
} HashType;

#define DIM(A) (sizeof(A) / sizeof(A[0]))

void out_of_memory(void) { abort(); }

#ifdef _HW_AES
#define ALG(x) x ## _hw
#else
#define ALG(x) x ## _sw
#endif

extern const ssh_cipheralg ALG(ssh_aes128_sdctr);
extern const ssh_cipheralg ALG(ssh_aes192_sdctr);
extern const ssh_cipheralg ALG(ssh_aes256_sdctr);
extern const ssh_cipheralg ALG(ssh_aes128_cbc);
extern const ssh_cipheralg ALG(ssh_aes192_cbc);
extern const ssh_cipheralg ALG(ssh_aes256_cbc);

static const ssh_cipheralg* get_alg(KeyType keytype, TestType testtype)
{
    if (testtype == SDCTR) switch (keytype) {
    case AES128: return &ALG(ssh_aes128_sdctr);
    case AES192: return &ALG(ssh_aes192_sdctr);
    case AES256: return &ALG(ssh_aes256_sdctr);
    default: return NULL;
    }

    switch (keytype) {
    case AES128: return &ALG(ssh_aes128_cbc);
    case AES192: return &ALG(ssh_aes192_cbc);
    case AES256: return &ALG(ssh_aes256_cbc);
    default: return NULL;
    }
}

static void test(KeyType keytype, TestType testtype, unsigned blocklen, FILE *file, unsigned char* ptr)
{
    const ssh_cipheralg* alg = get_alg(keytype, testtype);
    ssh_cipher* handle = ssh_cipher_new(alg);
    const size_t keylen = (size_t)keytype;
    unsigned char* const key = ptr + blocklen;
    unsigned char* const blk = ptr;
    unsigned char* const iv = ptr + keylen + blocklen;
    volatile unsigned long long now;

    ssh_cipher_setkey(handle, key);
    ssh_cipher_setiv(handle, iv);

    now = clock();
    switch (testtype)
    {
    case ENCRYPT:
    case SDCTR:
        ssh_cipher_encrypt(handle, blk, blocklen);
        break;
    case DECRYPT:
        ssh_cipher_encrypt(handle, blk, blocklen);
        break;
    }

    now = clock() - now + 1; // add a cycle to avoid zero
    fprintf(file, "%d\t%d\t%d\t%llu\n", testtype, keytype * 8, blocklen, now);

    ssh_cipher_free(handle);
}

#define MAXBLK (1 << 24)
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
    {
        printf("\n Block size %15i : ",b);
        fflush(stdout);
        for (i = 0; i < 30; ++i)
        {
            for (k = 0; k < keytypes_s; ++k) {
                test(keytypes[k], ENCRYPT, b, fp, ptr);
                test(keytypes[k], DECRYPT, b, fp, ptr);
                test(keytypes[k], SDCTR  , b, fp, ptr);
                fflush(fp);
            }
            printf("-");
            fflush(stdout);
        }
    }
    printf("\n");

    free(ptr);
    fclose(fp);

    return 0;
}
