#include<iostream>
#include<cstdint>
#include<string.h>
#define AES_ENCRYPT     1
#define AES_DECRYPT     0

/*
 * 32-bit integer manipulation macros (little endian)
 */
#ifndef GET_ULONG_LE
#define GET_ULONG_LE(n,b,i)                             \
{                                                       \
    (n) = ( (unsigned long) (b)[(i)    ]       )        \
        | ( (unsigned long) (b)[(i) + 1] <<  8 )        \
        | ( (unsigned long) (b)[(i) + 2] << 16 )        \
        | ( (unsigned long) (b)[(i) + 3] << 24 );       \
}
#endif

#ifndef PUT_ULONG_LE
#define PUT_ULONG_LE(n,b,i)                             \
{                                                       \
    (b)[(i)    ] = (unsigned char) ( (n)       );       \
    (b)[(i) + 1] = (unsigned char) ( (n) >>  8 );       \
    (b)[(i) + 2] = (unsigned char) ( (n) >> 16 );       \
    (b)[(i) + 3] = (unsigned char) ( (n) >> 24 );       \
}
#endif

#define ROTATE_LEFT(v,s) ( (v << s) | (v >> (32-s)) )

#define AES_FROUND(X0,X1,X2,X3,Y0,Y1,Y2,Y3)     \
{                                               \
    X0 = *RK++ ^ FT0[ ( Y0       ) & 0xFF ] ^   \
                 ROTATE_LEFT(FT0[ ( Y1 >>  8 ) & 0xFF ], 8) ^   \
                 ROTATE_LEFT(FT0[ ( Y2 >> 16 ) & 0xFF ],16) ^   \
                 ROTATE_LEFT(FT0[ ( Y3 >> 24 ) & 0xFF ],24);    \
                                                \
    X1 = *RK++ ^ FT0[ ( Y1       ) & 0xFF ] ^   \
                 ROTATE_LEFT(FT0[ ( Y2 >>  8 ) & 0xFF ], 8) ^   \
                 ROTATE_LEFT(FT0[ ( Y3 >> 16 ) & 0xFF ],16) ^   \
                 ROTATE_LEFT(FT0[ ( Y0 >> 24 ) & 0xFF ],24);    \
                                                \
    X2 = *RK++ ^ FT0[ ( Y2       ) & 0xFF ] ^   \
                 ROTATE_LEFT(FT0[ ( Y3 >>  8 ) & 0xFF ], 8) ^   \
                 ROTATE_LEFT(FT0[ ( Y0 >> 16 ) & 0xFF ],16) ^   \
                 ROTATE_LEFT(FT0[ ( Y1 >> 24 ) & 0xFF ],24);    \
                                                \
    X3 = *RK++ ^ FT0[ ( Y3       ) & 0xFF ] ^   \
                 ROTATE_LEFT(FT0[ ( Y0 >>  8 ) & 0xFF ], 8) ^   \
                 ROTATE_LEFT(FT0[ ( Y1 >> 16 ) & 0xFF ],16) ^   \
                 ROTATE_LEFT(FT0[ ( Y2 >> 24 ) & 0xFF ],24);    \
}

#define AES_RROUND(X0,X1,X2,X3,Y0,Y1,Y2,Y3)     \
{                                               \
    X0 = *RK++ ^ RT0[ ( Y0       ) & 0xFF ] ^   \
                 ROTATE_LEFT(RT0[ ( Y3 >>  8 ) & 0xFF ], 8) ^   \
                 ROTATE_LEFT(RT0[ ( Y2 >> 16 ) & 0xFF ],16) ^   \
                 ROTATE_LEFT(RT0[ ( Y1 >> 24 ) & 0xFF ],24);    \
                                                \
    X1 = *RK++ ^ RT0[ ( Y1       ) & 0xFF ] ^   \
                 ROTATE_LEFT(RT0[ ( Y0 >>  8 ) & 0xFF ], 8) ^   \
                 ROTATE_LEFT(RT0[ ( Y3 >> 16 ) & 0xFF ],16) ^   \
                 ROTATE_LEFT(RT0[ ( Y2 >> 24 ) & 0xFF ],24);    \
                                                \
    X2 = *RK++ ^ RT0[ ( Y2       ) & 0xFF ] ^   \
                 ROTATE_LEFT(RT0[ ( Y1 >>  8 ) & 0xFF ], 8) ^   \
                 ROTATE_LEFT(RT0[ ( Y0 >> 16 ) & 0xFF ],16) ^   \
                 ROTATE_LEFT(RT0[ ( Y3 >> 24 ) & 0xFF ],24);    \
                                                \
    X3 = *RK++ ^ RT0[ ( Y3       ) & 0xFF ] ^   \
                 ROTATE_LEFT(RT0[ ( Y2 >>  8 ) & 0xFF ], 8) ^   \
                 ROTATE_LEFT(RT0[ ( Y1 >> 16 ) & 0xFF ],16) ^   \
                 ROTATE_LEFT(RT0[ ( Y0 >> 24 ) & 0xFF ],24);    \
}
#define ROTL8(x) ( ( x << 8 ) & 0xFFFFFFFF ) | ( x >> 24 )
#define XTIME(x) ( ( x << 1 ) ^ ( ( x & 0x80 ) ? 0x1B : 0x00 ) )
#define MUL(x,y) ( ( x && y ) ? pow[(log[x]+log[y]) % 255] : 0 )

using namespace std;

/*
 * Forward S-box & tables
 */
static unsigned char FSb[256];
static unsigned long FT0[256]; 
static unsigned long FT1[256]; 
static unsigned long FT2[256]; 
static unsigned long FT3[256]; 

/*
 * Reverse S-box & tables
 */
static unsigned char RSb[256];
static unsigned long RT0[256];
static unsigned long RT1[256];
static unsigned long RT2[256];
static unsigned long RT3[256];

/*
 * Round constants
 */
static unsigned long RCON[10];
static int aes_init_done = 0;

typedef struct
{
    int nr;                     /*!<  number of rounds  */
    uint32_t *rk;               /*!<  AES round keys    */
    uint32_t buf[68];           /*!<  unaligned data    */
} aes_context;

int aes_setkey_enc(aes_context *ctx,
    const unsigned char *key, unsigned int keysize);
int aes_setkey_dec(aes_context *ctx,
    const unsigned char *key, unsigned int keysize);
static void aes_gen_tables();
int aes_ecb_ttable(aes_context *ctx,int mode,const uint8_t input[16],uint8_t output[16]);
