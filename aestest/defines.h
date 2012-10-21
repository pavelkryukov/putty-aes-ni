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

#define DIM(A) (sizeof(A) / sizeof(A[0]))
