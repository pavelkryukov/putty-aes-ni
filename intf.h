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

#ifndef PUTTY_AES_NI_AES_H
#define PUTTY_AES_NI_AES_H

#ifdef __cplusplus
extern "C" {
#endif

struct ssh_cipher;

struct ssh_cipher* create_handle(int keytype, int is_accelerated, unsigned char* key, unsigned char* iv);
void encrypt(struct ssh_cipher* handle, unsigned char* blk, size_t length);
void decrypt(struct ssh_cipher* handle, unsigned char* blk, size_t length);
void free_handle(struct ssh_cipher* handle);

#ifdef __cplusplus
}
#endif

#endif

