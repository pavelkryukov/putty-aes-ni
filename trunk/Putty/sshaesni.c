/**
 * sshaesni.c
 *
 * Implementation of AES for PuTTY using AES-NI
 * instuction set expansion.
 *
 * @author Pavel Kryukov <kryukov@frtk.ru>
 * @author Maxim Kuznetsov <maks.kuznetsov@gmail.com>
 * @author Svyatoslav Kuzmich <svatoslav1@gmail.com>
 * Based on sshaes.c source file of PuTTY
 *
 * For Putty AES NI project
 * http://putty-aes-ni.googlecode.com/
 */

#include <assert.h>
#include <stdlib.h>

#include <wmmintrin.h>
#include <smmintrin.h>

#include "ssh.h"

#define MAX_NR 14   /* max no of rounds */
#define NB 4        /* no of words in cipher blk */

typedef struct AESContext AESContext;

struct AESContext {
    word32 keysched[(MAX_NR + 2) * NB];
    word32 invkeysched[(MAX_NR + 2) * NB];
    word32 iv[NB];
    unsigned int Nr; /* number of rounds */
    unsigned int offset; /* offset for aligned key expansion */
};

static __m128i AES_128_ASSIST (__m128i temp1, __m128i temp2)
{
    __m128i temp3;
    temp2 = _mm_shuffle_epi32 (temp2 ,0xff);
    temp3 = _mm_slli_si128 (temp1, 0x4);
    temp1 = _mm_xor_si128 (temp1, temp3);
    temp3 = _mm_slli_si128 (temp3, 0x4);
    temp1 = _mm_xor_si128 (temp1, temp3);
    temp3 = _mm_slli_si128 (temp3, 0x4);
    temp1 = _mm_xor_si128 (temp1, temp3);
    temp1 = _mm_xor_si128 (temp1, temp2);
    return temp1;
}

static void KEY_192_ASSIST(__m128i* temp1, __m128i * temp2, __m128i * temp3)
{
    __m128i temp4;
    *temp2 = _mm_shuffle_epi32 (*temp2, 0x55);
    temp4 = _mm_slli_si128 (*temp1, 0x4);
    *temp1 = _mm_xor_si128 (*temp1, temp4);
    temp4 = _mm_slli_si128 (temp4, 0x4);
    *temp1 = _mm_xor_si128 (*temp1, temp4);
    temp4 = _mm_slli_si128 (temp4, 0x4);
    *temp1 = _mm_xor_si128 (*temp1, temp4);
    *temp1 = _mm_xor_si128 (*temp1, *temp2);
    *temp2 = _mm_shuffle_epi32(*temp1, 0xff);
    temp4 = _mm_slli_si128 (*temp3, 0x4);
    *temp3 = _mm_xor_si128 (*temp3, temp4);
    *temp3 = _mm_xor_si128 (*temp3, *temp2);
}

static void KEY_256_ASSIST_1(__m128i* temp1, __m128i * temp2)
{
    __m128i temp4;
    *temp2 = _mm_shuffle_epi32(*temp2, 0xff);
    temp4 = _mm_slli_si128 (*temp1, 0x4);
    *temp1 = _mm_xor_si128 (*temp1, temp4);
    temp4 = _mm_slli_si128 (temp4, 0x4);
    *temp1 = _mm_xor_si128 (*temp1, temp4);
    temp4 = _mm_slli_si128 (temp4, 0x4);
    *temp1 = _mm_xor_si128 (*temp1, temp4);
    *temp1 = _mm_xor_si128 (*temp1, *temp2);
}

static void KEY_256_ASSIST_2(__m128i* temp1, __m128i * temp3)
{
    __m128i temp2,temp4;
    temp4 = _mm_aeskeygenassist_si128 (*temp1, 0x0);
    temp2 = _mm_shuffle_epi32(temp4, 0xaa);
    temp4 = _mm_slli_si128 (*temp3, 0x4);
    *temp3 = _mm_xor_si128 (*temp3, temp4);
    temp4 = _mm_slli_si128 (temp4, 0x4);
    *temp3 = _mm_xor_si128 (*temp3, temp4);
    temp4 = _mm_slli_si128 (temp4, 0x4);
    *temp3 = _mm_xor_si128 (*temp3, temp4);
    *temp3 = _mm_xor_si128 (*temp3, temp2);
}

static void AES_128_Key_Expansion (unsigned char *userkey, __m128i *key)
{
    __m128i temp1, temp2;
    temp1 = _mm_loadu_si128((__m128i*)userkey);
    key[0] = temp1;
    temp2 = _mm_aeskeygenassist_si128 (temp1 ,0x1);
    temp1 = AES_128_ASSIST(temp1, temp2);
    key[1] = temp1;
    temp2 = _mm_aeskeygenassist_si128 (temp1,0x2);
    temp1 = AES_128_ASSIST(temp1, temp2);
    key[2] = temp1;
    temp2 = _mm_aeskeygenassist_si128 (temp1,0x4);
    temp1 = AES_128_ASSIST(temp1, temp2);
    key[3] = temp1;
    temp2 = _mm_aeskeygenassist_si128 (temp1,0x8);
    temp1 = AES_128_ASSIST(temp1, temp2);
    key[4] = temp1;
    temp2 = _mm_aeskeygenassist_si128 (temp1,0x10);
    temp1 = AES_128_ASSIST(temp1, temp2);
    key[5] = temp1;
    temp2 = _mm_aeskeygenassist_si128 (temp1,0x20);
    temp1 = AES_128_ASSIST(temp1, temp2);
    key[6] = temp1;
    temp2 = _mm_aeskeygenassist_si128 (temp1,0x40);
    temp1 = AES_128_ASSIST(temp1, temp2);
    key[7] = temp1;
    temp2 = _mm_aeskeygenassist_si128 (temp1,0x80);
    temp1 = AES_128_ASSIST(temp1, temp2);
    key[8] = temp1;
    temp2 = _mm_aeskeygenassist_si128 (temp1,0x1b);
    temp1 = AES_128_ASSIST(temp1, temp2);
    key[9] = temp1;
    temp2 = _mm_aeskeygenassist_si128 (temp1,0x36);
    temp1 = AES_128_ASSIST(temp1, temp2);
    key[10] = temp1;
}

static void AES_192_Key_Expansion (unsigned char *userkey, __m128i *key)
{
    __m128i temp1, temp2, temp3;
    temp1 = _mm_loadu_si128((__m128i*)userkey);
    temp3 = _mm_loadu_si128((__m128i*)(userkey+16));
    key[0]=temp1;
    key[1]=temp3;
    temp2=_mm_aeskeygenassist_si128 (temp3,0x1);
    KEY_192_ASSIST(&temp1, &temp2, &temp3);
    key[1] = (__m128i)_mm_shuffle_pd((__m128d)key[1],
            (__m128d)temp1,0);
    key[2] = (__m128i)_mm_shuffle_pd((__m128d)temp1,(__m128d)temp3,1);
    temp2=_mm_aeskeygenassist_si128 (temp3,0x2);
    KEY_192_ASSIST(&temp1, &temp2, &temp3);
    key[3]=temp1;
    key[4]=temp3;
    temp2=_mm_aeskeygenassist_si128 (temp3,0x4);
    KEY_192_ASSIST(&temp1, &temp2, &temp3);
    key[4] = (__m128i)_mm_shuffle_pd((__m128d)key[4],
            (__m128d)temp1,0);
    key[5] = (__m128i)_mm_shuffle_pd((__m128d)temp1,(__m128d)temp3,1);
    temp2=_mm_aeskeygenassist_si128 (temp3,0x8);
    KEY_192_ASSIST(&temp1, &temp2, &temp3);
    key[6]=temp1;
    key[7]=temp3;
    temp2=_mm_aeskeygenassist_si128 (temp3,0x10);
    KEY_192_ASSIST(&temp1, &temp2, &temp3);
    key[7] = (__m128i)_mm_shuffle_pd((__m128d)key[7],
            (__m128d)temp1,0);
    key[8] = (__m128i)_mm_shuffle_pd((__m128d)temp1,(__m128d)temp3,1);
    temp2=_mm_aeskeygenassist_si128 (temp3,0x20);
    KEY_192_ASSIST(&temp1, &temp2, &temp3);
    key[9]=temp1;
    key[10]=temp3;
    temp2=_mm_aeskeygenassist_si128 (temp3,0x40);
    KEY_192_ASSIST(&temp1, &temp2, &temp3);
    key[10] = (__m128i)_mm_shuffle_pd((__m128d)key[10],
            (__m128d)temp1,0);
    key[11] = (__m128i)_mm_shuffle_pd((__m128d)temp1,(__m128d)temp3,1);
    temp2=_mm_aeskeygenassist_si128 (temp3,0x80);
    KEY_192_ASSIST(&temp1, &temp2, &temp3);
    key[12]=temp1;
    key[13]=temp3;
}

static void AES_256_Key_Expansion (unsigned char *userkey, __m128i *key)
{
    __m128i temp1, temp2, temp3;
    temp1 = _mm_loadu_si128((__m128i*)userkey);
    temp3 = _mm_loadu_si128((__m128i*)(userkey+16));
    key[0] = temp1;
    key[1] = temp3;
    temp2 = _mm_aeskeygenassist_si128 (temp3,0x01);
    KEY_256_ASSIST_1(&temp1, &temp2);
    key[2]=temp1;
    KEY_256_ASSIST_2(&temp1, &temp3);
    key[3]=temp3;
    temp2 = _mm_aeskeygenassist_si128 (temp3,0x02);
    KEY_256_ASSIST_1(&temp1, &temp2);
    key[4]=temp1;
    KEY_256_ASSIST_2(&temp1, &temp3);
    key[5]=temp3;
    temp2 = _mm_aeskeygenassist_si128 (temp3,0x04);
    KEY_256_ASSIST_1(&temp1, &temp2);
    key[6]=temp1;
    KEY_256_ASSIST_2(&temp1, &temp3);
    key[7]=temp3;
    temp2 = _mm_aeskeygenassist_si128 (temp3,0x08);
    KEY_256_ASSIST_1(&temp1, &temp2);
    key[8]=temp1;
    KEY_256_ASSIST_2(&temp1, &temp3);
    key[9]=temp3;
    temp2 = _mm_aeskeygenassist_si128 (temp3,0x10);
    KEY_256_ASSIST_1(&temp1, &temp2);
    key[10]=temp1;
    KEY_256_ASSIST_2(&temp1, &temp3);
    key[11]=temp3;
    temp2 = _mm_aeskeygenassist_si128 (temp3,0x20);
    KEY_256_ASSIST_1(&temp1, &temp2);
    key[12]=temp1;
    KEY_256_ASSIST_2(&temp1, &temp3);
    key[13]=temp3;
    temp2 = _mm_aeskeygenassist_si128 (temp3,0x40);
    KEY_256_ASSIST_1(&temp1, &temp2);
    key[14]=temp1;
}

/*
 * Set up an AESContext. `keylen' and `blocklen' are measured in
 * bytes; each can be either 16 (128-bit), 24 (192-bit), or 32
 * (256-bit).
 */
static void aes_setup(AESContext * ctx, unsigned char *key, int keylen)
{
    __m128i *keysched, *invkeysched;
    ctx->offset = 16 - ((size_t)ctx % 16);
    keysched = (__m128i*)((unsigned char*)ctx->keysched + ctx->offset);
    invkeysched = (__m128i*)((unsigned char*)ctx->invkeysched + ctx->offset);

    ctx->Nr = 6 + (keylen / 4); /* Number of rounds */
    invkeysched += ctx->Nr;

    /*
     * Now do the key setup itself.
     */
    switch (keylen)
    {
    case 16:
        AES_128_Key_Expansion (key, keysched);
        break;
    case 24:
        AES_192_Key_Expansion (key, keysched);
        break;
    case 32:
        AES_256_Key_Expansion (key, keysched);
        break;
    default:
        assert(0);
    }

    /*
     * Now prepare the modified keys for the inverse cipher.
     */
    *invkeysched = *keysched;
    switch (ctx->Nr)
    {
    case 14:
        *(--invkeysched) = _mm_aesimc_si128(*(++keysched));
        *(--invkeysched) = _mm_aesimc_si128(*(++keysched));
    case 12:
        *(--invkeysched) = _mm_aesimc_si128(*(++keysched));
        *(--invkeysched) = _mm_aesimc_si128(*(++keysched));
    case 10:
        *(--invkeysched) = _mm_aesimc_si128(*(++keysched));
        *(--invkeysched) = _mm_aesimc_si128(*(++keysched));
        *(--invkeysched) = _mm_aesimc_si128(*(++keysched));
        *(--invkeysched) = _mm_aesimc_si128(*(++keysched));
        *(--invkeysched) = _mm_aesimc_si128(*(++keysched));
        *(--invkeysched) = _mm_aesimc_si128(*(++keysched));
        *(--invkeysched) = _mm_aesimc_si128(*(++keysched));
        *(--invkeysched) = _mm_aesimc_si128(*(++keysched));
        *(--invkeysched) = _mm_aesimc_si128(*(++keysched));
    default:
        *(--invkeysched) = *(++keysched);
    }
}

static void aes_encrypt_cbc(unsigned char *blk, int len, AESContext * ctx)
{
    __m128i enc;
    __m128i* block = (__m128i*)blk;
    const __m128i* finish = (__m128i*)(blk + len);

    assert((len & 15) == 0);

    /* Load IV */
    enc = _mm_loadu_si128((__m128i*)(ctx->iv));
    while (block < finish)
    {
        /* Key schedule ptr   */
        __m128i* keysched = (__m128i*)((unsigned char*)ctx->keysched + ctx->offset);

        /* Xor data with IV */
        enc  = _mm_xor_si128(_mm_loadu_si128(block), enc);

        /* Perform rounds */
        enc  = _mm_xor_si128(enc, *keysched);
        switch (ctx->Nr)
        {
        case 14:
            enc = _mm_aesenc_si128(enc, *(++keysched));
            enc = _mm_aesenc_si128(enc, *(++keysched));
        case 12:
            enc = _mm_aesenc_si128(enc, *(++keysched));
            enc = _mm_aesenc_si128(enc, *(++keysched));
        case 10:
            enc = _mm_aesenc_si128(enc, *(++keysched));
            enc = _mm_aesenc_si128(enc, *(++keysched));
            enc = _mm_aesenc_si128(enc, *(++keysched));
            enc = _mm_aesenc_si128(enc, *(++keysched));
            enc = _mm_aesenc_si128(enc, *(++keysched));
            enc = _mm_aesenc_si128(enc, *(++keysched));
            enc = _mm_aesenc_si128(enc, *(++keysched));
            enc = _mm_aesenc_si128(enc, *(++keysched));
            enc = _mm_aesenc_si128(enc, *(++keysched));
            enc = _mm_aesenclast_si128(enc, *(++keysched));
            break;
        default:
            assert(0);
        }

        /* Store and go to next block */
        _mm_storeu_si128(block, enc);
        ++block;
    }

    /* Update IV */
    _mm_storeu_si128((__m128i*)(ctx->iv), enc);
}

static void aes_decrypt_cbc(unsigned char *blk, int len, AESContext * ctx)
{
    __m128i dec, last, iv;
    __m128i* block = (__m128i*)blk;
    const __m128i* finish = (__m128i*)(blk + len);

    assert((len & 15) == 0);

    /* Load IV */
    iv = _mm_loadu_si128((__m128i*)(ctx->iv));
    while (block < finish)
    {
        /* Key schedule ptr   */
        __m128i* keysched = (__m128i*)((unsigned char*)ctx->invkeysched + ctx->offset);
        last = _mm_loadu_si128(block);
        dec  = _mm_xor_si128(last, *keysched);
        switch (ctx->Nr)
        {
        case 14:
            dec = _mm_aesdec_si128(dec, *(++keysched));
            dec = _mm_aesdec_si128(dec, *(++keysched));
        case 12:
            dec = _mm_aesdec_si128(dec, *(++keysched));
            dec = _mm_aesdec_si128(dec, *(++keysched));
        case 10:
            dec = _mm_aesdec_si128(dec, *(++keysched));
            dec = _mm_aesdec_si128(dec, *(++keysched));
            dec = _mm_aesdec_si128(dec, *(++keysched));
            dec = _mm_aesdec_si128(dec, *(++keysched));
            dec = _mm_aesdec_si128(dec, *(++keysched));
            dec = _mm_aesdec_si128(dec, *(++keysched));
            dec = _mm_aesdec_si128(dec, *(++keysched));
            dec = _mm_aesdec_si128(dec, *(++keysched));
            dec = _mm_aesdec_si128(dec, *(++keysched));
            dec = _mm_aesdeclast_si128(dec, *(++keysched));
            break;
        default:
            assert(0);
        }

        /* Xor data with IV */
        dec  = _mm_xor_si128(iv, dec);

        /* Store data */
        _mm_storeu_si128(block, dec);
        iv = last;

        /* Go to next block */
        ++block;
    }

    /* Update IV */
    _mm_storeu_si128((__m128i*)(ctx->iv), dec);
}

static void aes_sdctr(unsigned char *blk, int len, AESContext *ctx)
{
    const __m128i BSWAP_EPI64 = _mm_setr_epi8(3,2,1,0,7,6,5,4,11,10,9,8,15,14,13,12);
    const __m128i ONE  = _mm_setr_epi32(0,0,0,1);
    const __m128i ZERO = _mm_setzero_si128();
    __m128i iv;
    __m128i* block = (__m128i*)blk;
    const __m128i* finish = (__m128i*)(blk + len);

    assert((len & 15) == 0);

    iv = _mm_loadu_si128((__m128i*)ctx->iv);

    while (block < finish)
    {
        __m128i enc;
        __m128i* keysched = (__m128i*)((unsigned char*)ctx->keysched + ctx->offset);/* Key schedule ptr   */

        /* Perform rounds */
        enc  = _mm_xor_si128(iv, *keysched); /* Note that we use IV */
        switch (ctx->Nr)
        {
        case 14:
            enc = _mm_aesenc_si128(enc, *(++keysched));
            enc = _mm_aesenc_si128(enc, *(++keysched));
        case 12:
            enc = _mm_aesenc_si128(enc, *(++keysched));
            enc = _mm_aesenc_si128(enc, *(++keysched));
        case 10:
            enc = _mm_aesenc_si128(enc, *(++keysched));
            enc = _mm_aesenc_si128(enc, *(++keysched));
            enc = _mm_aesenc_si128(enc, *(++keysched));
            enc = _mm_aesenc_si128(enc, *(++keysched));
            enc = _mm_aesenc_si128(enc, *(++keysched));
            enc = _mm_aesenc_si128(enc, *(++keysched));
            enc = _mm_aesenc_si128(enc, *(++keysched));
            enc = _mm_aesenc_si128(enc, *(++keysched));
            enc = _mm_aesenc_si128(enc, *(++keysched));
            enc = _mm_aesenclast_si128(enc, *(++keysched));
            break;
        default:
            assert(0);
        }

        /* Xor with block and store result */
        enc = _mm_xor_si128(enc, _mm_loadu_si128(block));
        _mm_storeu_si128(block, enc);

        /* Increment of IV */
        iv  = _mm_shuffle_epi8(iv, BSWAP_EPI64); /* Swap endianess     */
        iv  = _mm_add_epi64(iv, ONE);            /* Inc low part       */
        enc = _mm_cmpeq_epi64(iv, ZERO);         /* Check for carry    */
        enc = _mm_unpacklo_epi64(ZERO, enc);     /* Pack carry reg     */
        iv  = _mm_sub_epi64(iv, enc);            /* Sub carry reg      */
        iv  = _mm_shuffle_epi8(iv, BSWAP_EPI64); /* Swap enianess back */

        /* Go to next block */
        ++block;
    }

    /* Update IV */
    _mm_storeu_si128((__m128i*)ctx->iv, iv);
}

void *aes_make_context(void)
{
    return snew(AESContext);
}

void aes_free_context(void *handle)
{
    sfree(handle);
}

void aes128_key(void *handle, unsigned char *key)
{
    AESContext *ctx = (AESContext *)handle;
    aes_setup(ctx, key, 16);
}

void aes192_key(void *handle, unsigned char *key)
{
    AESContext *ctx = (AESContext *)handle;
    aes_setup(ctx, key, 24);
}

void aes256_key(void *handle, unsigned char *key)
{
    AESContext *ctx = (AESContext *)handle;
    aes_setup(ctx, key, 32);
}

void aes_iv(void *handle, unsigned char *iv)
{
    AESContext *ctx = (AESContext *)handle;
    memcpy(ctx->iv, iv, sizeof(ctx->iv));
}

void aes_ssh2_encrypt_blk(void *handle, unsigned char *blk, int len)
{
    AESContext *ctx = (AESContext *)handle;
    aes_encrypt_cbc(blk, len, ctx);
}

void aes_ssh2_decrypt_blk(void *handle, unsigned char *blk, int len)
{
    AESContext *ctx = (AESContext *)handle;
    aes_decrypt_cbc(blk, len, ctx);
}

static void aes_ssh2_sdctr(void *handle, unsigned char *blk, int len)
{
    AESContext *ctx = (AESContext *)handle;
    aes_sdctr(blk, len, ctx);
}

void aes256_encrypt_pubkey(unsigned char *key, unsigned char *blk, int len)
{
    AESContext ctx;
    aes_setup(&ctx, key, 32);
    memset(ctx.iv, 0, sizeof(ctx.iv));
    aes_encrypt_cbc(blk, len, &ctx);
    memset(&ctx, 0, sizeof(ctx));
}

void aes256_decrypt_pubkey(unsigned char *key, unsigned char *blk, int len)
{
    AESContext ctx;
    aes_setup(&ctx, key, 32);
    memset(ctx.iv, 0, sizeof(ctx.iv));
    aes_decrypt_cbc(blk, len, &ctx);
    memset(&ctx, 0, sizeof(ctx));
}

static const struct ssh2_cipher ssh_aes128_ctr = {
    aes_make_context, aes_free_context, aes_iv, aes128_key,
    aes_ssh2_sdctr, aes_ssh2_sdctr,
    "aes128-ctr",
    16, 128, 0, "AES-128 SDCTR"
};

static const struct ssh2_cipher ssh_aes192_ctr = {
    aes_make_context, aes_free_context, aes_iv, aes192_key,
    aes_ssh2_sdctr, aes_ssh2_sdctr,
    "aes192-ctr",
    16, 192, 0, "AES-192 SDCTR"
};

static const struct ssh2_cipher ssh_aes256_ctr = {
    aes_make_context, aes_free_context, aes_iv, aes256_key,
    aes_ssh2_sdctr, aes_ssh2_sdctr,
    "aes256-ctr",
    16, 256, 0, "AES-256 SDCTR"
};

static const struct ssh2_cipher ssh_aes128 = {
    aes_make_context, aes_free_context, aes_iv, aes128_key,
    aes_ssh2_encrypt_blk, aes_ssh2_decrypt_blk,
    "aes128-cbc",
    16, 128, SSH_CIPHER_IS_CBC, "AES-128 CBC"
};

static const struct ssh2_cipher ssh_aes192 = {
    aes_make_context, aes_free_context, aes_iv, aes192_key,
    aes_ssh2_encrypt_blk, aes_ssh2_decrypt_blk,
    "aes192-cbc",
    16, 192, SSH_CIPHER_IS_CBC, "AES-192 CBC"
};

static const struct ssh2_cipher ssh_aes256 = {
    aes_make_context, aes_free_context, aes_iv, aes256_key,
    aes_ssh2_encrypt_blk, aes_ssh2_decrypt_blk,
    "aes256-cbc",
    16, 256, SSH_CIPHER_IS_CBC, "AES-256 CBC"
};

static const struct ssh2_cipher ssh_rijndael_lysator = {
    aes_make_context, aes_free_context, aes_iv, aes256_key,
    aes_ssh2_encrypt_blk, aes_ssh2_decrypt_blk,
    "rijndael-cbc@lysator.liu.se",
    16, 256, SSH_CIPHER_IS_CBC, "AES-256 CBC"
};

static const struct ssh2_cipher *const aes_list[] = {
    &ssh_aes256_ctr,
    &ssh_aes256,
    &ssh_rijndael_lysator,
    &ssh_aes192_ctr,
    &ssh_aes192,
    &ssh_aes128_ctr,
    &ssh_aes128,
};

const struct ssh2_ciphers ssh2_aes = {
    sizeof(aes_list) / sizeof(*aes_list),
    aes_list
};
