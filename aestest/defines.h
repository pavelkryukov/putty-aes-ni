/**
 * defines.h
 *
 * Common defines for AES cryptoalgorithm
 *
 * @author kryukov@frtk.ru
 * @version 1.0
 *
 * For Putty AES NI project
 * http://putty-aes-ni.googlecode.com/
 */

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

#define DIM(A) (sizeof(A) / sizeof(A[0]))
