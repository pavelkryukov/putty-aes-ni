/**
 * aesdemo.c
 *
 * Demonstration of AES cryptoalgorithm
 *
 * @author kryukov@frtk.ru
 * @version 1.5
 *
 * For Putty AES NI project
 * http://putty-aes-ni.googlecode.com/
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "coverage.h"

size_t readfile(char* filename, unsigned char** ptr)
{
    int size = 0;
    FILE *f = fopen(filename, "rb");
    if (f == NULL) 
    { 
        *ptr = NULL;
        return -1;
    } 
    fseek(f, 0, SEEK_END);
    size = ftell(f);
    fseek(f, 0, SEEK_SET);
    *ptr = (unsigned char *)malloc(size+1);
    if (size != fread(*ptr, sizeof(unsigned char), size, f)) 
    { 
        free(*ptr);
        return -2;
    } 
    fclose(f);
    (*ptr)[size] = 0;
    return size;
}

void writefile(char* filename, unsigned char* ptr, size_t size)
{
    FILE *f = fopen(filename, "w");
    fwrite(ptr, sizeof(unsigned char), size, f);
    fclose(f);
}

void encode(unsigned char* block, size_t blocklen)
{
    unsigned char key[32] = "imtheoperatorwithmypocketcalcula";
    unsigned char iv[16] = "initializationve";
    void *handle = aes_make_context();
    blocklen &= ~15ull;
    aes256_key(handle, key);
    aes_iv(handle, iv);
    aes_ssh2_encrypt_blk(handle, block, blocklen);
    aes_free_context(handle);
}

void decode(unsigned char* block, size_t blocklen)
{
    unsigned char key[32] = "imtheoperatorwithmypocketcalcula";
    unsigned char iv[16] = "initializationve";
    void *handle = aes_make_context();
    blocklen &= ~15ull;
    aes256_key(handle, key);
    aes_iv(handle, iv);
    aes_ssh2_decrypt_blk(handle, block, blocklen);
    aes_free_context(handle);
}

int main(int argc, char** argv)
{
    int size = 0;
    unsigned char* block;
    if (argc != 3)
    {
        fprintf(stderr,
            "Syntax error! First parameter is source file, second is dest file\n");
        return 1;
    }
    size = readfile(argv[1], &block);
    if (size == -1)
    {
        fprintf(stderr, "%s is not found!\n", argv[1]);
        return 1;
    }
    if (size == -2) {
        fprintf(stderr, "Error in reading %s\n", argv[1]);
        return 1;
    }
#ifdef DECODE
    decode(block, size);
#else
    encode(block, size);
#endif
    writefile(argv[2], block, size);
    free(block);
    return 0;
}