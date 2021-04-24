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

#include "ssh.h"
#include "intf.h"

void out_of_memory() { abort(); }

static const struct ssh_cipheralg* get_alg(int keytype, int is_accelerated)
{
    switch (keytype) {
    	case 16: return is_accelerated ? &ssh_aes128_cbc_ni : &ssh_aes128_cbc_sw;
    	case 24: return is_accelerated ? &ssh_aes192_cbc_ni : &ssh_aes192_cbc_sw;
    	case 32: return is_accelerated ? &ssh_aes256_cbc_ni : &ssh_aes256_cbc_sw;
    	default: return NULL;
    }
}

struct ssh_cipher* create_handle(int keytype, int is_accelerated, unsigned char* key, unsigned char* iv)
{
    const ssh_cipheralg* alg = get_alg(keytype, is_accelerated);
    ssh_cipher* handle = ssh_cipher_new(alg);
    ssh_cipher_setkey(handle, key);
    ssh_cipher_setiv(handle, iv);
    return handle;
}

void encrypt(struct ssh_cipher* handle, unsigned char* blk, size_t length)
{
    ssh_cipher_encrypt(handle, blk, length);
}

void decrypt(struct ssh_cipher* handle, unsigned char* blk, size_t length)
{
    ssh_cipher_decrypt(handle, blk, length);
}

void free_handle(struct ssh_cipher* handle)
{
    ssh_cipher_free(handle);
}

