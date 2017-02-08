#include"aes_ttable.hpp"

int aes_setkey_enc(aes_context *ctx, const unsigned char *key, unsigned int keysize) {
    unsigned int i;
    uint32_t *RK;

    if( aes_init_done == 0 )
    {
        aes_gen_tables();
        aes_init_done = 1;

    }

    switch( keysize )
    {
        case 128: ctx->nr = 10; break;
        case 192: ctx->nr = 12; break;
        case 256: ctx->nr = 14; break;
        default: printf("key size not imeplemented\n"); break;
    }

    ctx->rk = RK = ctx->buf;

    for( i = 0; i < (keysize >> 5); i++ )
    {
        GET_ULONG_LE( RK[i], key, i << 2 );
    }

    switch( ctx->nr )
    {
        case 10:

            for( i = 0; i < 10; i++, RK += 4 )
            {
                RK[4]  = RK[0] ^ RCON[i] ^
                ( (unsigned long) FSb[ ( RK[3] >>  8 ) & 0xFF ]       ) ^
                ( (unsigned long) FSb[ ( RK[3] >> 16 ) & 0xFF ] <<  8 ) ^
                ( (unsigned long) FSb[ ( RK[3] >> 24 ) & 0xFF ] << 16 ) ^
                ( (unsigned long) FSb[ ( RK[3]       ) & 0xFF ] << 24 );

                RK[5]  = RK[1] ^ RK[4];
                RK[6]  = RK[2] ^ RK[5];
                RK[7]  = RK[3] ^ RK[6];
            }
            break;

        case 12:

            for( i = 0; i < 8; i++, RK += 6 )
            {
                RK[6]  = RK[0] ^ RCON[i] ^
                ( (unsigned long) FSb[ ( RK[5] >>  8 ) & 0xFF ]       ) ^
                ( (unsigned long) FSb[ ( RK[5] >> 16 ) & 0xFF ] <<  8 ) ^
                ( (unsigned long) FSb[ ( RK[5] >> 24 ) & 0xFF ] << 16 ) ^
                ( (unsigned long) FSb[ ( RK[5]       ) & 0xFF ] << 24 );

                RK[7]  = RK[1] ^ RK[6];
                RK[8]  = RK[2] ^ RK[7];
                RK[9]  = RK[3] ^ RK[8];
                RK[10] = RK[4] ^ RK[9];
                RK[11] = RK[5] ^ RK[10];
            }
            break;

        case 14:

            for( i = 0; i < 7; i++, RK += 8 )
            {
                RK[8]  = RK[0] ^ RCON[i] ^
                ( (unsigned long) FSb[ ( RK[7] >>  8 ) & 0xFF ]       ) ^
                ( (unsigned long) FSb[ ( RK[7] >> 16 ) & 0xFF ] <<  8 ) ^
                ( (unsigned long) FSb[ ( RK[7] >> 24 ) & 0xFF ] << 16 ) ^
                ( (unsigned long) FSb[ ( RK[7]       ) & 0xFF ] << 24 );

                RK[9]  = RK[1] ^ RK[8];
                RK[10] = RK[2] ^ RK[9];
                RK[11] = RK[3] ^ RK[10];

                RK[12] = RK[4] ^
                ( (unsigned long) FSb[ ( RK[11]       ) & 0xFF ]       ) ^
                ( (unsigned long) FSb[ ( RK[11] >>  8 ) & 0xFF ] <<  8 ) ^
                ( (unsigned long) FSb[ ( RK[11] >> 16 ) & 0xFF ] << 16 ) ^
                ( (unsigned long) FSb[ ( RK[11] >> 24 ) & 0xFF ] << 24 );

                RK[13] = RK[5] ^ RK[12];
                RK[14] = RK[6] ^ RK[13];
                RK[15] = RK[7] ^ RK[14];
            }
            break;

        default:

            break;
    }

    return( 0 );
}

static void aes_gen_tables()
{
    int i, x, y, z;
    int pow[256];
    int log[256];

    /*
     * compute pow and log tables over GF(2^8)
     */
    for( i = 0, x = 1; i < 256; i++ )
    {
        pow[i] = x;
        log[x] = i;
        x = ( x ^ XTIME( x ) ) & 0xFF;
    }

    /*
     * calculate the round constants
     */
    for( i = 0, x = 1; i < 10; i++ )
    {
        RCON[i] = (unsigned long) x;
        x = XTIME( x ) & 0xFF;
    }

    /*
     * generate the forward and reverse S-boxes
     */
    FSb[0x00] = 0x63;
    RSb[0x63] = 0x00;

    for( i = 1; i < 256; i++ )
    {
        x = pow[255 - log[i]];

        y  = x; y = ( (y << 1) | (y >> 7) ) & 0xFF;
        x ^= y; y = ( (y << 1) | (y >> 7) ) & 0xFF;
        x ^= y; y = ( (y << 1) | (y >> 7) ) & 0xFF;
        x ^= y; y = ( (y << 1) | (y >> 7) ) & 0xFF;
        x ^= y ^ 0x63;

        FSb[i] = (unsigned char) x;
        RSb[x] = (unsigned char) i;
    }

    /*
     * generate the forward and reverse tables
     */
    for( i = 0; i < 256; i++ )
    {
        x = FSb[i];
        y = XTIME( x ) & 0xFF;
        z =  ( y ^ x ) & 0xFF;

        FT0[i] = ( (unsigned long) y       ) ^
                 ( (unsigned long) x <<  8 ) ^
                 ( (unsigned long) x << 16 ) ^
                 ( (unsigned long) z << 24 );

        FT1[i] = ROTL8( FT0[i] );
        FT2[i] = ROTL8( FT1[i] );
        FT3[i] = ROTL8( FT2[i] );

        x = RSb[i];

        RT0[i] = ( (unsigned long) MUL( 0x0E, x )       ) ^
                 ( (unsigned long) MUL( 0x09, x ) <<  8 ) ^
                 ( (unsigned long) MUL( 0x0D, x ) << 16 ) ^
                 ( (unsigned long) MUL( 0x0B, x ) << 24 );

        RT1[i] = ROTL8( RT0[i] );
        RT2[i] = ROTL8( RT1[i] );
        RT3[i] = ROTL8( RT2[i] );
    }
}

int aes_setkey_dec(aes_context *ctx, const unsigned char *key, unsigned int keysize) {
    int i, j;
    aes_context cty;
    uint32_t *RK;
    uint32_t *SK;
    int ret;

    switch( keysize )
    {
        case 128: ctx->nr = 10; break;
        case 192: ctx->nr = 12; break;
        case 256: ctx->nr = 14; break;
        default : printf("Invalid key length\n"); break;
    }
    ctx->rk = RK = ctx->buf;

    ret = aes_setkey_enc( &cty, key, keysize );
    if( ret != 0 )
        return( ret );

    SK = cty.rk + cty.nr * 4;

    *RK++ = *SK++;
    *RK++ = *SK++;
    *RK++ = *SK++;
    *RK++ = *SK++;

    for( i = ctx->nr - 1, SK -= 8; i > 0; i--, SK -= 8 )
    {
        for( j = 0; j < 4; j++, SK++ )
        {
            *RK++ = RT0[ FSb[ ( *SK       ) & 0xFF ] ] ^
                    RT1[ FSb[ ( *SK >>  8 ) & 0xFF ] ] ^
                    RT2[ FSb[ ( *SK >> 16 ) & 0xFF ] ] ^
                    RT3[ FSb[ ( *SK >> 24 ) & 0xFF ] ];
        }
    }

    *RK++ = *SK++;
    *RK++ = *SK++;
    *RK++ = *SK++;
    *RK++ = *SK++;

    memset( &cty, 0, sizeof( aes_context ) );

    return( 0 );
}

/*
 * AES-ECB block encryption/decryption
 */
int aes_ecb_ttable( aes_context *ctx,
                    int mode,
                    const uint8_t input[16],
                    uint8_t output[16] )
{
    int i;
    uint32_t *RK, X0, X1, X2, X3, Y0, Y1, Y2, Y3;

    RK = ctx->rk;

    GET_ULONG_LE( X0, input,  0 ); X0 ^= *RK++;
    GET_ULONG_LE( X1, input,  4 ); X1 ^= *RK++;
    GET_ULONG_LE( X2, input,  8 ); X2 ^= *RK++;
    GET_ULONG_LE( X3, input, 12 ); X3 ^= *RK++;

    if( mode == AES_DECRYPT )
    {
        for( i = (ctx->nr >> 1) - 1; i > 0; i-- )
        {
            AES_RROUND( Y0, Y1, Y2, Y3, X0, X1, X2, X3 );
            AES_RROUND( X0, X1, X2, X3, Y0, Y1, Y2, Y3 );
        }

        AES_RROUND( Y0, Y1, Y2, Y3, X0, X1, X2, X3 );

        X0 = *RK++ ^ \
                ( (unsigned long) RSb[ ( Y0       ) & 0xFF ]       ) ^
                ( (unsigned long) RSb[ ( Y3 >>  8 ) & 0xFF ] <<  8 ) ^
                ( (unsigned long) RSb[ ( Y2 >> 16 ) & 0xFF ] << 16 ) ^
                ( (unsigned long) RSb[ ( Y1 >> 24 ) & 0xFF ] << 24 );

        X1 = *RK++ ^ \
                ( (unsigned long) RSb[ ( Y1       ) & 0xFF ]       ) ^
                ( (unsigned long) RSb[ ( Y0 >>  8 ) & 0xFF ] <<  8 ) ^
                ( (unsigned long) RSb[ ( Y3 >> 16 ) & 0xFF ] << 16 ) ^
                ( (unsigned long) RSb[ ( Y2 >> 24 ) & 0xFF ] << 24 );

        X2 = *RK++ ^ \
                ( (unsigned long) RSb[ ( Y2       ) & 0xFF ]       ) ^
                ( (unsigned long) RSb[ ( Y1 >>  8 ) & 0xFF ] <<  8 ) ^
                ( (unsigned long) RSb[ ( Y0 >> 16 ) & 0xFF ] << 16 ) ^
                ( (unsigned long) RSb[ ( Y3 >> 24 ) & 0xFF ] << 24 );

        X3 = *RK++ ^ \
                ( (unsigned long) RSb[ ( Y3       ) & 0xFF ]       ) ^
                ( (unsigned long) RSb[ ( Y2 >>  8 ) & 0xFF ] <<  8 ) ^
                ( (unsigned long) RSb[ ( Y1 >> 16 ) & 0xFF ] << 16 ) ^
                ( (unsigned long) RSb[ ( Y0 >> 24 ) & 0xFF ] << 24 );
    }
    else /* AES_ENCRYPT */
    {
        for( i = (ctx->nr >> 1) - 1; i > 0; i-- )
        {
            AES_FROUND( Y0, Y1, Y2, Y3, X0, X1, X2, X3 );
            AES_FROUND( X0, X1, X2, X3, Y0, Y1, Y2, Y3 );
        }

        AES_FROUND( Y0, Y1, Y2, Y3, X0, X1, X2, X3 );

        X0 = *RK++ ^ \
                ( (unsigned long) FSb[ ( Y0       ) & 0xFF ]       ) ^
                ( (unsigned long) FSb[ ( Y1 >>  8 ) & 0xFF ] <<  8 ) ^
                ( (unsigned long) FSb[ ( Y2 >> 16 ) & 0xFF ] << 16 ) ^
                ( (unsigned long) FSb[ ( Y3 >> 24 ) & 0xFF ] << 24 );

        X1 = *RK++ ^ \
                ( (unsigned long) FSb[ ( Y1       ) & 0xFF ]       ) ^
                ( (unsigned long) FSb[ ( Y2 >>  8 ) & 0xFF ] <<  8 ) ^
                ( (unsigned long) FSb[ ( Y3 >> 16 ) & 0xFF ] << 16 ) ^
                ( (unsigned long) FSb[ ( Y0 >> 24 ) & 0xFF ] << 24 );

        X2 = *RK++ ^ \
                ( (unsigned long) FSb[ ( Y2       ) & 0xFF ]       ) ^
                ( (unsigned long) FSb[ ( Y3 >>  8 ) & 0xFF ] <<  8 ) ^
                ( (unsigned long) FSb[ ( Y0 >> 16 ) & 0xFF ] << 16 ) ^
                ( (unsigned long) FSb[ ( Y1 >> 24 ) & 0xFF ] << 24 );

        X3 = *RK++ ^ \
                ( (unsigned long) FSb[ ( Y3       ) & 0xFF ]       ) ^
                ( (unsigned long) FSb[ ( Y0 >>  8 ) & 0xFF ] <<  8 ) ^
                ( (unsigned long) FSb[ ( Y1 >> 16 ) & 0xFF ] << 16 ) ^
                ( (unsigned long) FSb[ ( Y2 >> 24 ) & 0xFF ] << 24 );
    }

    PUT_ULONG_LE( X0, output,  0 );
    PUT_ULONG_LE( X1, output,  4 );
    PUT_ULONG_LE( X2, output,  8 );
    PUT_ULONG_LE( X3, output, 12 );

    return( 0 );
}