/**
 * sshaesni.c
 * 
 * Implementation of AES / Rijndael for PuTTY
 * Base on sshaes.c source file of PuTTY
 *
 * @author Putty AES NI team.
 *
 * For Putty AES NI project
 * http://putty-aes-ni.googlecode.com/
 */

#include <assert.h>
#include <stdlib.h>

#include <wmmintrin.h>

#include "ssh.h"

#define MAX_NR 14		       /* max no of rounds */
#define MAX_NK 8		       /* max no of words in input key */
#define MAX_NB 8		       /* max no of words in cipher blk */

#define mulby2(x) ( ((x&0x7F) << 1) ^ (x & 0x80 ? 0x1B : 0) )

typedef struct AESContext AESContext;

struct AESContext {
    word32 keysched[(MAX_NR + 1) * MAX_NB];
    word32 invkeysched[(MAX_NR + 1) * MAX_NB];
    void (*decrypt) (AESContext * ctx, word32 * block);
    word32 iv[MAX_NB];
    int Nb, Nr;
};

static const unsigned char Sbox[256] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5,
    0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0,
    0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc,
    0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a,
    0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0,
    0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b,
    0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85,
    0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5,
    0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17,
    0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88,
    0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c,
    0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9,
    0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6,
    0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e,
    0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94,
    0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68,
    0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

static const unsigned char Sboxinv[256] = {
    0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38,
    0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
    0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87,
    0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
    0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d,
    0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
    0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2,
    0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
    0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16,
    0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
    0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda,
    0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
    0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a,
    0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
    0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02,
    0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
    0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea,
    0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
    0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85,
    0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
    0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89,
    0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
    0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20,
    0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
    0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31,
    0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
    0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d,
    0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
    0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0,
    0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26,
    0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d
};

static const word32 D0[256] = {
    0x51f4a750, 0x7e416553, 0x1a17a4c3, 0x3a275e96,
    0x3bab6bcb, 0x1f9d45f1, 0xacfa58ab, 0x4be30393,
    0x2030fa55, 0xad766df6, 0x88cc7691, 0xf5024c25,
    0x4fe5d7fc, 0xc52acbd7, 0x26354480, 0xb562a38f,
    0xdeb15a49, 0x25ba1b67, 0x45ea0e98, 0x5dfec0e1,
    0xc32f7502, 0x814cf012, 0x8d4697a3, 0x6bd3f9c6,
    0x038f5fe7, 0x15929c95, 0xbf6d7aeb, 0x955259da,
    0xd4be832d, 0x587421d3, 0x49e06929, 0x8ec9c844,
    0x75c2896a, 0xf48e7978, 0x99583e6b, 0x27b971dd,
    0xbee14fb6, 0xf088ad17, 0xc920ac66, 0x7dce3ab4,
    0x63df4a18, 0xe51a3182, 0x97513360, 0x62537f45,
    0xb16477e0, 0xbb6bae84, 0xfe81a01c, 0xf9082b94,
    0x70486858, 0x8f45fd19, 0x94de6c87, 0x527bf8b7,
    0xab73d323, 0x724b02e2, 0xe31f8f57, 0x6655ab2a,
    0xb2eb2807, 0x2fb5c203, 0x86c57b9a, 0xd33708a5,
    0x302887f2, 0x23bfa5b2, 0x02036aba, 0xed16825c,
    0x8acf1c2b, 0xa779b492, 0xf307f2f0, 0x4e69e2a1,
    0x65daf4cd, 0x0605bed5, 0xd134621f, 0xc4a6fe8a,
    0x342e539d, 0xa2f355a0, 0x058ae132, 0xa4f6eb75,
    0x0b83ec39, 0x4060efaa, 0x5e719f06, 0xbd6e1051,
    0x3e218af9, 0x96dd063d, 0xdd3e05ae, 0x4de6bd46,
    0x91548db5, 0x71c45d05, 0x0406d46f, 0x605015ff,
    0x1998fb24, 0xd6bde997, 0x894043cc, 0x67d99e77,
    0xb0e842bd, 0x07898b88, 0xe7195b38, 0x79c8eedb,
    0xa17c0a47, 0x7c420fe9, 0xf8841ec9, 0x00000000,
    0x09808683, 0x322bed48, 0x1e1170ac, 0x6c5a724e,
    0xfd0efffb, 0x0f853856, 0x3daed51e, 0x362d3927,
    0x0a0fd964, 0x685ca621, 0x9b5b54d1, 0x24362e3a,
    0x0c0a67b1, 0x9357e70f, 0xb4ee96d2, 0x1b9b919e,
    0x80c0c54f, 0x61dc20a2, 0x5a774b69, 0x1c121a16,
    0xe293ba0a, 0xc0a02ae5, 0x3c22e043, 0x121b171d,
    0x0e090d0b, 0xf28bc7ad, 0x2db6a8b9, 0x141ea9c8,
    0x57f11985, 0xaf75074c, 0xee99ddbb, 0xa37f60fd,
    0xf701269f, 0x5c72f5bc, 0x44663bc5, 0x5bfb7e34,
    0x8b432976, 0xcb23c6dc, 0xb6edfc68, 0xb8e4f163,
    0xd731dcca, 0x42638510, 0x13972240, 0x84c61120,
    0x854a247d, 0xd2bb3df8, 0xaef93211, 0xc729a16d,
    0x1d9e2f4b, 0xdcb230f3, 0x0d8652ec, 0x77c1e3d0,
    0x2bb3166c, 0xa970b999, 0x119448fa, 0x47e96422,
    0xa8fc8cc4, 0xa0f03f1a, 0x567d2cd8, 0x223390ef,
    0x87494ec7, 0xd938d1c1, 0x8ccaa2fe, 0x98d40b36,
    0xa6f581cf, 0xa57ade28, 0xdab78e26, 0x3fadbfa4,
    0x2c3a9de4, 0x5078920d, 0x6a5fcc9b, 0x547e4662,
    0xf68d13c2, 0x90d8b8e8, 0x2e39f75e, 0x82c3aff5,
    0x9f5d80be, 0x69d0937c, 0x6fd52da9, 0xcf2512b3,
    0xc8ac993b, 0x10187da7, 0xe89c636e, 0xdb3bbb7b,
    0xcd267809, 0x6e5918f4, 0xec9ab701, 0x834f9aa8,
    0xe6956e65, 0xaaffe67e, 0x21bccf08, 0xef15e8e6,
    0xbae79bd9, 0x4a6f36ce, 0xea9f09d4, 0x29b07cd6,
    0x31a4b2af, 0x2a3f2331, 0xc6a59430, 0x35a266c0,
    0x744ebc37, 0xfc82caa6, 0xe090d0b0, 0x33a7d815,
    0xf104984a, 0x41ecdaf7, 0x7fcd500e, 0x1791f62f,
    0x764dd68d, 0x43efb04d, 0xccaa4d54, 0xe49604df,
    0x9ed1b5e3, 0x4c6a881b, 0xc12c1fb8, 0x4665517f,
    0x9d5eea04, 0x018c355d, 0xfa877473, 0xfb0b412e,
    0xb3671d5a, 0x92dbd252, 0xe9105633, 0x6dd64713,
    0x9ad7618c, 0x37a10c7a, 0x59f8148e, 0xeb133c89,
    0xcea927ee, 0xb761c935, 0xe11ce5ed, 0x7a47b13c,
    0x9cd2df59, 0x55f2733f, 0x1814ce79, 0x73c737bf,
    0x53f7cdea, 0x5ffdaa5b, 0xdf3d6f14, 0x7844db86,
    0xcaaff381, 0xb968c43e, 0x3824342c, 0xc2a3405f,
    0x161dc372, 0xbce2250c, 0x283c498b, 0xff0d9541,
    0x39a80171, 0x080cb3de, 0xd8b4e49c, 0x6456c190,
    0x7bcb8461, 0xd532b670, 0x486c5c74, 0xd0b85742,
};
static const word32 D1[256] = {
    0x5051f4a7, 0x537e4165, 0xc31a17a4, 0x963a275e,
    0xcb3bab6b, 0xf11f9d45, 0xabacfa58, 0x934be303,
    0x552030fa, 0xf6ad766d, 0x9188cc76, 0x25f5024c,
    0xfc4fe5d7, 0xd7c52acb, 0x80263544, 0x8fb562a3,
    0x49deb15a, 0x6725ba1b, 0x9845ea0e, 0xe15dfec0,
    0x02c32f75, 0x12814cf0, 0xa38d4697, 0xc66bd3f9,
    0xe7038f5f, 0x9515929c, 0xebbf6d7a, 0xda955259,
    0x2dd4be83, 0xd3587421, 0x2949e069, 0x448ec9c8,
    0x6a75c289, 0x78f48e79, 0x6b99583e, 0xdd27b971,
    0xb6bee14f, 0x17f088ad, 0x66c920ac, 0xb47dce3a,
    0x1863df4a, 0x82e51a31, 0x60975133, 0x4562537f,
    0xe0b16477, 0x84bb6bae, 0x1cfe81a0, 0x94f9082b,
    0x58704868, 0x198f45fd, 0x8794de6c, 0xb7527bf8,
    0x23ab73d3, 0xe2724b02, 0x57e31f8f, 0x2a6655ab,
    0x07b2eb28, 0x032fb5c2, 0x9a86c57b, 0xa5d33708,
    0xf2302887, 0xb223bfa5, 0xba02036a, 0x5ced1682,
    0x2b8acf1c, 0x92a779b4, 0xf0f307f2, 0xa14e69e2,
    0xcd65daf4, 0xd50605be, 0x1fd13462, 0x8ac4a6fe,
    0x9d342e53, 0xa0a2f355, 0x32058ae1, 0x75a4f6eb,
    0x390b83ec, 0xaa4060ef, 0x065e719f, 0x51bd6e10,
    0xf93e218a, 0x3d96dd06, 0xaedd3e05, 0x464de6bd,
    0xb591548d, 0x0571c45d, 0x6f0406d4, 0xff605015,
    0x241998fb, 0x97d6bde9, 0xcc894043, 0x7767d99e,
    0xbdb0e842, 0x8807898b, 0x38e7195b, 0xdb79c8ee,
    0x47a17c0a, 0xe97c420f, 0xc9f8841e, 0x00000000,
    0x83098086, 0x48322bed, 0xac1e1170, 0x4e6c5a72,
    0xfbfd0eff, 0x560f8538, 0x1e3daed5, 0x27362d39,
    0x640a0fd9, 0x21685ca6, 0xd19b5b54, 0x3a24362e,
    0xb10c0a67, 0x0f9357e7, 0xd2b4ee96, 0x9e1b9b91,
    0x4f80c0c5, 0xa261dc20, 0x695a774b, 0x161c121a,
    0x0ae293ba, 0xe5c0a02a, 0x433c22e0, 0x1d121b17,
    0x0b0e090d, 0xadf28bc7, 0xb92db6a8, 0xc8141ea9,
    0x8557f119, 0x4caf7507, 0xbbee99dd, 0xfda37f60,
    0x9ff70126, 0xbc5c72f5, 0xc544663b, 0x345bfb7e,
    0x768b4329, 0xdccb23c6, 0x68b6edfc, 0x63b8e4f1,
    0xcad731dc, 0x10426385, 0x40139722, 0x2084c611,
    0x7d854a24, 0xf8d2bb3d, 0x11aef932, 0x6dc729a1,
    0x4b1d9e2f, 0xf3dcb230, 0xec0d8652, 0xd077c1e3,
    0x6c2bb316, 0x99a970b9, 0xfa119448, 0x2247e964,
    0xc4a8fc8c, 0x1aa0f03f, 0xd8567d2c, 0xef223390,
    0xc787494e, 0xc1d938d1, 0xfe8ccaa2, 0x3698d40b,
    0xcfa6f581, 0x28a57ade, 0x26dab78e, 0xa43fadbf,
    0xe42c3a9d, 0x0d507892, 0x9b6a5fcc, 0x62547e46,
    0xc2f68d13, 0xe890d8b8, 0x5e2e39f7, 0xf582c3af,
    0xbe9f5d80, 0x7c69d093, 0xa96fd52d, 0xb3cf2512,
    0x3bc8ac99, 0xa710187d, 0x6ee89c63, 0x7bdb3bbb,
    0x09cd2678, 0xf46e5918, 0x01ec9ab7, 0xa8834f9a,
    0x65e6956e, 0x7eaaffe6, 0x0821bccf, 0xe6ef15e8,
    0xd9bae79b, 0xce4a6f36, 0xd4ea9f09, 0xd629b07c,
    0xaf31a4b2, 0x312a3f23, 0x30c6a594, 0xc035a266,
    0x37744ebc, 0xa6fc82ca, 0xb0e090d0, 0x1533a7d8,
    0x4af10498, 0xf741ecda, 0x0e7fcd50, 0x2f1791f6,
    0x8d764dd6, 0x4d43efb0, 0x54ccaa4d, 0xdfe49604,
    0xe39ed1b5, 0x1b4c6a88, 0xb8c12c1f, 0x7f466551,
    0x049d5eea, 0x5d018c35, 0x73fa8774, 0x2efb0b41,
    0x5ab3671d, 0x5292dbd2, 0x33e91056, 0x136dd647,
    0x8c9ad761, 0x7a37a10c, 0x8e59f814, 0x89eb133c,
    0xeecea927, 0x35b761c9, 0xede11ce5, 0x3c7a47b1,
    0x599cd2df, 0x3f55f273, 0x791814ce, 0xbf73c737,
    0xea53f7cd, 0x5b5ffdaa, 0x14df3d6f, 0x867844db,
    0x81caaff3, 0x3eb968c4, 0x2c382434, 0x5fc2a340,
    0x72161dc3, 0x0cbce225, 0x8b283c49, 0x41ff0d95,
    0x7139a801, 0xde080cb3, 0x9cd8b4e4, 0x906456c1,
    0x617bcb84, 0x70d532b6, 0x74486c5c, 0x42d0b857,
};
static const word32 D2[256] = {
    0xa75051f4, 0x65537e41, 0xa4c31a17, 0x5e963a27,
    0x6bcb3bab, 0x45f11f9d, 0x58abacfa, 0x03934be3,
    0xfa552030, 0x6df6ad76, 0x769188cc, 0x4c25f502,
    0xd7fc4fe5, 0xcbd7c52a, 0x44802635, 0xa38fb562,
    0x5a49deb1, 0x1b6725ba, 0x0e9845ea, 0xc0e15dfe,
    0x7502c32f, 0xf012814c, 0x97a38d46, 0xf9c66bd3,
    0x5fe7038f, 0x9c951592, 0x7aebbf6d, 0x59da9552,
    0x832dd4be, 0x21d35874, 0x692949e0, 0xc8448ec9,
    0x896a75c2, 0x7978f48e, 0x3e6b9958, 0x71dd27b9,
    0x4fb6bee1, 0xad17f088, 0xac66c920, 0x3ab47dce,
    0x4a1863df, 0x3182e51a, 0x33609751, 0x7f456253,
    0x77e0b164, 0xae84bb6b, 0xa01cfe81, 0x2b94f908,
    0x68587048, 0xfd198f45, 0x6c8794de, 0xf8b7527b,
    0xd323ab73, 0x02e2724b, 0x8f57e31f, 0xab2a6655,
    0x2807b2eb, 0xc2032fb5, 0x7b9a86c5, 0x08a5d337,
    0x87f23028, 0xa5b223bf, 0x6aba0203, 0x825ced16,
    0x1c2b8acf, 0xb492a779, 0xf2f0f307, 0xe2a14e69,
    0xf4cd65da, 0xbed50605, 0x621fd134, 0xfe8ac4a6,
    0x539d342e, 0x55a0a2f3, 0xe132058a, 0xeb75a4f6,
    0xec390b83, 0xefaa4060, 0x9f065e71, 0x1051bd6e,
    0x8af93e21, 0x063d96dd, 0x05aedd3e, 0xbd464de6,
    0x8db59154, 0x5d0571c4, 0xd46f0406, 0x15ff6050,
    0xfb241998, 0xe997d6bd, 0x43cc8940, 0x9e7767d9,
    0x42bdb0e8, 0x8b880789, 0x5b38e719, 0xeedb79c8,
    0x0a47a17c, 0x0fe97c42, 0x1ec9f884, 0x00000000,
    0x86830980, 0xed48322b, 0x70ac1e11, 0x724e6c5a,
    0xfffbfd0e, 0x38560f85, 0xd51e3dae, 0x3927362d,
    0xd9640a0f, 0xa621685c, 0x54d19b5b, 0x2e3a2436,
    0x67b10c0a, 0xe70f9357, 0x96d2b4ee, 0x919e1b9b,
    0xc54f80c0, 0x20a261dc, 0x4b695a77, 0x1a161c12,
    0xba0ae293, 0x2ae5c0a0, 0xe0433c22, 0x171d121b,
    0x0d0b0e09, 0xc7adf28b, 0xa8b92db6, 0xa9c8141e,
    0x198557f1, 0x074caf75, 0xddbbee99, 0x60fda37f,
    0x269ff701, 0xf5bc5c72, 0x3bc54466, 0x7e345bfb,
    0x29768b43, 0xc6dccb23, 0xfc68b6ed, 0xf163b8e4,
    0xdccad731, 0x85104263, 0x22401397, 0x112084c6,
    0x247d854a, 0x3df8d2bb, 0x3211aef9, 0xa16dc729,
    0x2f4b1d9e, 0x30f3dcb2, 0x52ec0d86, 0xe3d077c1,
    0x166c2bb3, 0xb999a970, 0x48fa1194, 0x642247e9,
    0x8cc4a8fc, 0x3f1aa0f0, 0x2cd8567d, 0x90ef2233,
    0x4ec78749, 0xd1c1d938, 0xa2fe8cca, 0x0b3698d4,
    0x81cfa6f5, 0xde28a57a, 0x8e26dab7, 0xbfa43fad,
    0x9de42c3a, 0x920d5078, 0xcc9b6a5f, 0x4662547e,
    0x13c2f68d, 0xb8e890d8, 0xf75e2e39, 0xaff582c3,
    0x80be9f5d, 0x937c69d0, 0x2da96fd5, 0x12b3cf25,
    0x993bc8ac, 0x7da71018, 0x636ee89c, 0xbb7bdb3b,
    0x7809cd26, 0x18f46e59, 0xb701ec9a, 0x9aa8834f,
    0x6e65e695, 0xe67eaaff, 0xcf0821bc, 0xe8e6ef15,
    0x9bd9bae7, 0x36ce4a6f, 0x09d4ea9f, 0x7cd629b0,
    0xb2af31a4, 0x23312a3f, 0x9430c6a5, 0x66c035a2,
    0xbc37744e, 0xcaa6fc82, 0xd0b0e090, 0xd81533a7,
    0x984af104, 0xdaf741ec, 0x500e7fcd, 0xf62f1791,
    0xd68d764d, 0xb04d43ef, 0x4d54ccaa, 0x04dfe496,
    0xb5e39ed1, 0x881b4c6a, 0x1fb8c12c, 0x517f4665,
    0xea049d5e, 0x355d018c, 0x7473fa87, 0x412efb0b,
    0x1d5ab367, 0xd25292db, 0x5633e910, 0x47136dd6,
    0x618c9ad7, 0x0c7a37a1, 0x148e59f8, 0x3c89eb13,
    0x27eecea9, 0xc935b761, 0xe5ede11c, 0xb13c7a47,
    0xdf599cd2, 0x733f55f2, 0xce791814, 0x37bf73c7,
    0xcdea53f7, 0xaa5b5ffd, 0x6f14df3d, 0xdb867844,
    0xf381caaf, 0xc43eb968, 0x342c3824, 0x405fc2a3,
    0xc372161d, 0x250cbce2, 0x498b283c, 0x9541ff0d,
    0x017139a8, 0xb3de080c, 0xe49cd8b4, 0xc1906456,
    0x84617bcb, 0xb670d532, 0x5c74486c, 0x5742d0b8,
};
static const word32 D3[256] = {
    0xf4a75051, 0x4165537e, 0x17a4c31a, 0x275e963a,
    0xab6bcb3b, 0x9d45f11f, 0xfa58abac, 0xe303934b,
    0x30fa5520, 0x766df6ad, 0xcc769188, 0x024c25f5,
    0xe5d7fc4f, 0x2acbd7c5, 0x35448026, 0x62a38fb5,
    0xb15a49de, 0xba1b6725, 0xea0e9845, 0xfec0e15d,
    0x2f7502c3, 0x4cf01281, 0x4697a38d, 0xd3f9c66b,
    0x8f5fe703, 0x929c9515, 0x6d7aebbf, 0x5259da95,
    0xbe832dd4, 0x7421d358, 0xe0692949, 0xc9c8448e,
    0xc2896a75, 0x8e7978f4, 0x583e6b99, 0xb971dd27,
    0xe14fb6be, 0x88ad17f0, 0x20ac66c9, 0xce3ab47d,
    0xdf4a1863, 0x1a3182e5, 0x51336097, 0x537f4562,
    0x6477e0b1, 0x6bae84bb, 0x81a01cfe, 0x082b94f9,
    0x48685870, 0x45fd198f, 0xde6c8794, 0x7bf8b752,
    0x73d323ab, 0x4b02e272, 0x1f8f57e3, 0x55ab2a66,
    0xeb2807b2, 0xb5c2032f, 0xc57b9a86, 0x3708a5d3,
    0x2887f230, 0xbfa5b223, 0x036aba02, 0x16825ced,
    0xcf1c2b8a, 0x79b492a7, 0x07f2f0f3, 0x69e2a14e,
    0xdaf4cd65, 0x05bed506, 0x34621fd1, 0xa6fe8ac4,
    0x2e539d34, 0xf355a0a2, 0x8ae13205, 0xf6eb75a4,
    0x83ec390b, 0x60efaa40, 0x719f065e, 0x6e1051bd,
    0x218af93e, 0xdd063d96, 0x3e05aedd, 0xe6bd464d,
    0x548db591, 0xc45d0571, 0x06d46f04, 0x5015ff60,
    0x98fb2419, 0xbde997d6, 0x4043cc89, 0xd99e7767,
    0xe842bdb0, 0x898b8807, 0x195b38e7, 0xc8eedb79,
    0x7c0a47a1, 0x420fe97c, 0x841ec9f8, 0x00000000,
    0x80868309, 0x2bed4832, 0x1170ac1e, 0x5a724e6c,
    0x0efffbfd, 0x8538560f, 0xaed51e3d, 0x2d392736,
    0x0fd9640a, 0x5ca62168, 0x5b54d19b, 0x362e3a24,
    0x0a67b10c, 0x57e70f93, 0xee96d2b4, 0x9b919e1b,
    0xc0c54f80, 0xdc20a261, 0x774b695a, 0x121a161c,
    0x93ba0ae2, 0xa02ae5c0, 0x22e0433c, 0x1b171d12,
    0x090d0b0e, 0x8bc7adf2, 0xb6a8b92d, 0x1ea9c814,
    0xf1198557, 0x75074caf, 0x99ddbbee, 0x7f60fda3,
    0x01269ff7, 0x72f5bc5c, 0x663bc544, 0xfb7e345b,
    0x4329768b, 0x23c6dccb, 0xedfc68b6, 0xe4f163b8,
    0x31dccad7, 0x63851042, 0x97224013, 0xc6112084,
    0x4a247d85, 0xbb3df8d2, 0xf93211ae, 0x29a16dc7,
    0x9e2f4b1d, 0xb230f3dc, 0x8652ec0d, 0xc1e3d077,
    0xb3166c2b, 0x70b999a9, 0x9448fa11, 0xe9642247,
    0xfc8cc4a8, 0xf03f1aa0, 0x7d2cd856, 0x3390ef22,
    0x494ec787, 0x38d1c1d9, 0xcaa2fe8c, 0xd40b3698,
    0xf581cfa6, 0x7ade28a5, 0xb78e26da, 0xadbfa43f,
    0x3a9de42c, 0x78920d50, 0x5fcc9b6a, 0x7e466254,
    0x8d13c2f6, 0xd8b8e890, 0x39f75e2e, 0xc3aff582,
    0x5d80be9f, 0xd0937c69, 0xd52da96f, 0x2512b3cf,
    0xac993bc8, 0x187da710, 0x9c636ee8, 0x3bbb7bdb,
    0x267809cd, 0x5918f46e, 0x9ab701ec, 0x4f9aa883,
    0x956e65e6, 0xffe67eaa, 0xbccf0821, 0x15e8e6ef,
    0xe79bd9ba, 0x6f36ce4a, 0x9f09d4ea, 0xb07cd629,
    0xa4b2af31, 0x3f23312a, 0xa59430c6, 0xa266c035,
    0x4ebc3774, 0x82caa6fc, 0x90d0b0e0, 0xa7d81533,
    0x04984af1, 0xecdaf741, 0xcd500e7f, 0x91f62f17,
    0x4dd68d76, 0xefb04d43, 0xaa4d54cc, 0x9604dfe4,
    0xd1b5e39e, 0x6a881b4c, 0x2c1fb8c1, 0x65517f46,
    0x5eea049d, 0x8c355d01, 0x877473fa, 0x0b412efb,
    0x671d5ab3, 0xdbd25292, 0x105633e9, 0xd647136d,
    0xd7618c9a, 0xa10c7a37, 0xf8148e59, 0x133c89eb,
    0xa927eece, 0x61c935b7, 0x1ce5ede1, 0x47b13c7a,
    0xd2df599c, 0xf2733f55, 0x14ce7918, 0xc737bf73,
    0xf7cdea53, 0xfdaa5b5f, 0x3d6f14df, 0x44db8678,
    0xaff381ca, 0x68c43eb9, 0x24342c38, 0xa3405fc2,
    0x1dc37216, 0xe2250cbc, 0x3c498b28, 0x0d9541ff,
    0xa8017139, 0x0cb3de08, 0xb4e49cd8, 0x56c19064,
    0xcb84617b, 0x32b670d5, 0x6c5c7448, 0xb85742d0,
};

/*
 * Common macros in both the encryption and decryption routines.
 */
#define ADD_ROUND_KEY_4 (block[0]^=*keysched++, block[1]^=*keysched++, \
		         block[2]^=*keysched++, block[3]^=*keysched++)
#define ADD_ROUND_KEY_6 (block[0]^=*keysched++, block[1]^=*keysched++, \
		         block[2]^=*keysched++, block[3]^=*keysched++, \
		         block[4]^=*keysched++, block[5]^=*keysched++)
#define ADD_ROUND_KEY_8 (block[0]^=*keysched++, block[1]^=*keysched++, \
		         block[2]^=*keysched++, block[3]^=*keysched++, \
		         block[4]^=*keysched++, block[5]^=*keysched++, \
		         block[6]^=*keysched++, block[7]^=*keysched++)
#define MOVEWORD(i) ( block[i] = newstate[i] )

/*
 * Macros for the decryption routine. There are three decryption
 * cores, for Nb=4,6,8.
 */
#define MAKEWORD(i) ( newstate[i] = (D0[(block[i] >> 24) & 0xFF] ^ \
				     D1[(block[(i+C1)%Nb] >> 16) & 0xFF] ^ \
				     D2[(block[(i+C2)%Nb] >> 8) & 0xFF] ^ \
				     D3[block[(i+C3)%Nb] & 0xFF]) )
#define LASTWORD(i) (newstate[i] = (Sboxinv[(block[i] >> 24) & 0xFF] << 24) | \
			   (Sboxinv[(block[(i+C1)%Nb] >> 16) & 0xFF] << 16) | \
			   (Sboxinv[(block[(i+C2)%Nb] >>  8) & 0xFF] <<  8) | \
			   (Sboxinv[(block[(i+C3)%Nb]      ) & 0xFF]      ) )

/*
 * Core decrypt routines, expecting word32 inputs read big-endian
 * from the byte-oriented input stream.
 */
static void aes_decrypt_nb_4(AESContext * ctx, word32 * block)
{
    int i;
    static const int C1 = 4 - 1, C2 = 4 - 2, C3 = 4 - 3, Nb = 4;
    word32 *keysched = ctx->invkeysched;
    word32 newstate[4];
    for (i = 0; i < ctx->Nr - 1; i++) {
	ADD_ROUND_KEY_4;
	MAKEWORD(0);
	MAKEWORD(1);
	MAKEWORD(2);
	MAKEWORD(3);
	MOVEWORD(0);
	MOVEWORD(1);
	MOVEWORD(2);
	MOVEWORD(3);
    }
    ADD_ROUND_KEY_4;
    LASTWORD(0);
    LASTWORD(1);
    LASTWORD(2);
    LASTWORD(3);
    MOVEWORD(0);
    MOVEWORD(1);
    MOVEWORD(2);
    MOVEWORD(3);
    ADD_ROUND_KEY_4;
}
static void aes_decrypt_nb_6(AESContext * ctx, word32 * block)
{
    int i;
    static const int C1 = 6 - 1, C2 = 6 - 2, C3 = 6 - 3, Nb = 6;
    word32 *keysched = ctx->invkeysched;
    word32 newstate[6];
    for (i = 0; i < ctx->Nr - 1; i++) {
	ADD_ROUND_KEY_6;
	MAKEWORD(0);
	MAKEWORD(1);
	MAKEWORD(2);
	MAKEWORD(3);
	MAKEWORD(4);
	MAKEWORD(5);
	MOVEWORD(0);
	MOVEWORD(1);
	MOVEWORD(2);
	MOVEWORD(3);
	MOVEWORD(4);
	MOVEWORD(5);
    }
    ADD_ROUND_KEY_6;
    LASTWORD(0);
    LASTWORD(1);
    LASTWORD(2);
    LASTWORD(3);
    LASTWORD(4);
    LASTWORD(5);
    MOVEWORD(0);
    MOVEWORD(1);
    MOVEWORD(2);
    MOVEWORD(3);
    MOVEWORD(4);
    MOVEWORD(5);
    ADD_ROUND_KEY_6;
}
static void aes_decrypt_nb_8(AESContext * ctx, word32 * block)
{
    int i;
    static const int C1 = 8 - 1, C2 = 8 - 3, C3 = 8 - 4, Nb = 8;
    word32 *keysched = ctx->invkeysched;
    word32 newstate[8];
    for (i = 0; i < ctx->Nr - 1; i++) {
	ADD_ROUND_KEY_8;
	MAKEWORD(0);
	MAKEWORD(1);
	MAKEWORD(2);
	MAKEWORD(3);
	MAKEWORD(4);
	MAKEWORD(5);
	MAKEWORD(6);
	MAKEWORD(7);
	MOVEWORD(0);
	MOVEWORD(1);
	MOVEWORD(2);
	MOVEWORD(3);
	MOVEWORD(4);
	MOVEWORD(5);
	MOVEWORD(6);
	MOVEWORD(7);
    }
    ADD_ROUND_KEY_8;
    LASTWORD(0);
    LASTWORD(1);
    LASTWORD(2);
    LASTWORD(3);
    LASTWORD(4);
    LASTWORD(5);
    LASTWORD(6);
    LASTWORD(7);
    MOVEWORD(0);
    MOVEWORD(1);
    MOVEWORD(2);
    MOVEWORD(3);
    MOVEWORD(4);
    MOVEWORD(5);
    MOVEWORD(6);
    MOVEWORD(7);
    ADD_ROUND_KEY_8;
}

#undef MAKEWORD
#undef LASTWORD

static void aes_wrap_bytes(unsigned char *blk)
{
    blk[0] ^= blk[3];
    blk[3] ^= blk[0];
    blk[0] ^= blk[3];
    blk[1] ^= blk[2];
    blk[2] ^= blk[1];
    blk[1] ^= blk[2];
}

/*
 * Set up an AESContext. `keylen' and `blocklen' are measured in
 * bytes; each can be either 16 (128-bit), 24 (192-bit), or 32
 * (256-bit).
 */
static void aes_setup(AESContext * ctx, int blocklen,
	       unsigned char *key, int keylen)
{
    int i, j, Nk, rconst;

    assert(blocklen == 16 || blocklen == 24 || blocklen == 32);
    assert(keylen == 16 || keylen == 24 || keylen == 32);

    /*
     * Basic parameters. Words per block, words in key, rounds.
     */
    Nk = keylen / 4;
    ctx->Nb = blocklen / 4;
    ctx->Nr = 6 + (ctx->Nb > Nk ? ctx->Nb : Nk);

    /*
     * Assign core-function pointers.
     */
    if (ctx->Nb == 8)
	ctx->decrypt = aes_decrypt_nb_8;
    else if (ctx->Nb == 6)
	ctx->decrypt = aes_decrypt_nb_6;
    else if (ctx->Nb == 4)
	ctx->decrypt = aes_decrypt_nb_4;

    /*
     * Now do the key setup itself.
     */
    rconst = 1;
    for (i = 0; i < (ctx->Nr + 1) * ctx->Nb; i++) {
	if (i < Nk)
	    ctx->keysched[i] = GET_32BIT_MSB_FIRST(key + 4 * i);
	else {
	    word32 temp = ctx->keysched[i - 1];
	    if (i % Nk == 0) {
		int a, b, c, d;
		a = (temp >> 16) & 0xFF;
		b = (temp >> 8) & 0xFF;
		c = (temp >> 0) & 0xFF;
		d = (temp >> 24) & 0xFF;
		temp = Sbox[a] ^ rconst;
		temp = (temp << 8) | Sbox[b];
		temp = (temp << 8) | Sbox[c];
		temp = (temp << 8) | Sbox[d];
		rconst = mulby2(rconst);
	    } else if (i % Nk == 4 && Nk > 6) {
		int a, b, c, d;
		a = (temp >> 24) & 0xFF;
		b = (temp >> 16) & 0xFF;
		c = (temp >> 8) & 0xFF;
		d = (temp >> 0) & 0xFF;
		temp = Sbox[a];
		temp = (temp << 8) | Sbox[b];
		temp = (temp << 8) | Sbox[c];
		temp = (temp << 8) | Sbox[d];
	    }
	    ctx->keysched[i] = ctx->keysched[i - Nk] ^ temp;
	}
    }

    /*
     * Now prepare the modified keys for the inverse cipher.
     */
    for (i = 0; i <= ctx->Nr; i++) {
	for (j = 0; j < ctx->Nb; j++) {
	    word32 temp;
	    temp = ctx->keysched[(ctx->Nr - i) * ctx->Nb + j];
	    if (i != 0 && i != ctx->Nr) {
		/*
		 * Perform the InvMixColumn operation on i. The D
		 * tables give the result of InvMixColumn applied
		 * to Sboxinv on individual bytes, so we should
		 * compose Sbox with the D tables for this.
		 */
		int a, b, c, d;
		a = (temp >> 24) & 0xFF;
		b = (temp >> 16) & 0xFF;
		c = (temp >> 8) & 0xFF;
		d = (temp >> 0) & 0xFF;
		temp = D0[Sbox[a]];
		temp ^= D1[Sbox[b]];
		temp ^= D2[Sbox[c]];
		temp ^= D3[Sbox[d]];
	    }
	    ctx->invkeysched[i * ctx->Nb + j] = temp;
	}
    }
    
    for (i = 0; i < (MAX_NR + 1) * MAX_NB; ++i)
        aes_wrap_bytes((unsigned char*)(&ctx->keysched[i]));
}

static void aes_decrypt(AESContext * ctx, word32 * block)
{
    ctx->decrypt(ctx, block);
}

static void aes_encrypt_cbc(unsigned char *blk, int len, AESContext * ctx)
{
    __m128i enc;
    __m128i* block = (__m128i*)blk;
    int i;

    assert((len & 15) == 0);

    len /= 16;

    /* Load IV */
    enc = _mm_loadu_si128((__m128i*)(ctx->iv));
    for (i = 0; i < len; ++i)
    {
        /* Key schedule ptr   */
        __m128i* keysched = (__m128i*)(ctx->keysched);

        /* Xor data with IV */
        enc  = _mm_xor_si128(_mm_loadu_si128(block), enc);
        
        /* Perform rounds */
        enc  = _mm_xor_si128(enc, *(keysched++));
        switch (ctx->Nr)
        {
        case 14: 
            enc = _mm_aesenc_si128(enc, *(keysched++));
            enc = _mm_aesenc_si128(enc, *(keysched++));
        case 12:
            enc = _mm_aesenc_si128(enc, *(keysched++));
            enc = _mm_aesenc_si128(enc, *(keysched++));
        case 10:
            enc = _mm_aesenc_si128(enc, *(keysched++));
            enc = _mm_aesenc_si128(enc, *(keysched++));
            enc = _mm_aesenc_si128(enc, *(keysched++));
            enc = _mm_aesenc_si128(enc, *(keysched++));
            enc = _mm_aesenc_si128(enc, *(keysched++));
            enc = _mm_aesenc_si128(enc, *(keysched++));
            enc = _mm_aesenc_si128(enc, *(keysched++));
            enc = _mm_aesenc_si128(enc, *(keysched++));
            enc = _mm_aesenc_si128(enc, *(keysched++));
            enc = _mm_aesenclast_si128(enc, *(keysched++));
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
    word32 iv[4], x[4], ct[4];
    int i;

    assert((len & 15) == 0);

    memcpy(iv, ctx->iv, sizeof(iv));

    while (len > 0) {
	for (i = 0; i < 4; i++)
	    x[i] = ct[i] = GET_32BIT_MSB_FIRST(blk + 4 * i);
	aes_decrypt(ctx, x);
	for (i = 0; i < 4; i++) {
	    PUT_32BIT_MSB_FIRST(blk + 4 * i, iv[i] ^ x[i]);
	    iv[i] = ct[i];
	}
	blk += 16;
	len -= 16;
    }

    memcpy(ctx->iv, iv, sizeof(iv));
}

static void aes_sdctr(unsigned char *blk, int len, AESContext *ctx)
{
    __m128i iv;
    __m128i* block = (__m128i*)blk;
    int i;

    assert((len & 15) == 0);
    len /= 16;
    
    iv = _mm_loadu_si128((__m128i*)ctx->iv);
    
    for (i = 0; i < len; ++i)
    {
        __m128i enc;
        __m128i* keysched = (__m128i*)(ctx->keysched);/* Key schedule ptr   */

        /* Perform rounds */
        enc  = _mm_xor_si128(iv, *(keysched++)); /* Note that we use IV */
        switch (ctx->Nr)
        {
        case 14: 
            enc = _mm_aesenc_si128(enc, *(keysched++));
            enc = _mm_aesenc_si128(enc, *(keysched++));
        case 12:
            enc = _mm_aesenc_si128(enc, *(keysched++));
            enc = _mm_aesenc_si128(enc, *(keysched++));
        case 10:
            enc = _mm_aesenc_si128(enc, *(keysched++));
            enc = _mm_aesenc_si128(enc, *(keysched++));
            enc = _mm_aesenc_si128(enc, *(keysched++));
            enc = _mm_aesenc_si128(enc, *(keysched++));
            enc = _mm_aesenc_si128(enc, *(keysched++));
            enc = _mm_aesenc_si128(enc, *(keysched++));
            enc = _mm_aesenc_si128(enc, *(keysched++));
            enc = _mm_aesenc_si128(enc, *(keysched++));
            enc = _mm_aesenc_si128(enc, *(keysched++));
            enc = _mm_aesenclast_si128(enc, *(keysched++));
            break;
        default:
            assert(0);
        }

        /* Xor with block and store result */
        enc = _mm_xor_si128(enc, _mm_loadu_si128(block));
        _mm_storeu_si128(block, enc);                 

        /* Update of IV. FIXME: we need to simplify it */
        { 
            word32 temp[4];
            int k;
            _mm_storeu_si128((__m128i*)temp, iv);
            for (k = 3; k >= 0; k--)
                if ((temp[k] = (temp[k] + (1ul << 24)) & 0xffffffff) != 0)
                    break;
            iv = _mm_loadu_si128((__m128i*)temp);
        }
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
    aes_setup(ctx, 16, key, 16);
}

void aes192_key(void *handle, unsigned char *key)
{
    AESContext *ctx = (AESContext *)handle;
    aes_setup(ctx, 16, key, 24);
}

void aes256_key(void *handle, unsigned char *key)
{
    AESContext *ctx = (AESContext *)handle;
    aes_setup(ctx, 16, key, 32);
}

void aes_iv(void *handle, unsigned char *iv)
{
    AESContext *ctx = (AESContext *)handle;
    int i;
    for (i = 0; i < 4; i++)
	ctx->iv[i] = GET_32BIT_LSB_FIRST(iv + 4 * i);
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

void aes_ssh2_sdctr(void *handle, unsigned char *blk, int len)
{
    AESContext *ctx = (AESContext *)handle;
    aes_sdctr(blk, len, ctx);
}

void aes256_encrypt_pubkey(unsigned char *key, unsigned char *blk, int len)
{
    AESContext ctx;
    aes_setup(&ctx, 16, key, 32);
    memset(ctx.iv, 0, sizeof(ctx.iv));
    aes_encrypt_cbc(blk, len, &ctx);
    smemclr(&ctx, sizeof(ctx));
}

void aes256_decrypt_pubkey(unsigned char *key, unsigned char *blk, int len)
{
    AESContext ctx;
    aes_setup(&ctx, 16, key, 32);
    memset(ctx.iv, 0, sizeof(ctx.iv));
    aes_decrypt_cbc(blk, len, &ctx);
    smemclr(&ctx, sizeof(ctx));
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
