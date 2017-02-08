#include<iostream>
#include<iomanip>
#include<stdio.h>
#include <stdlib.h>
#include"aes_ttable.hpp"

using namespace std;

/**
 * Test executable for AES t-table
 */

int main(int argc,char*argv[]) {
    if(argc<2) {
        cout<<"Usage: ./aes_test.out numberOfAESCipher"<<endl;
        exit(0);
    }
    int max=atoi(argv[1]);
    uint8_t plain[16]={0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0,0xa0};
    uint8_t key[16]={0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
    aes_context ctx;
    aes_setkey_enc(&ctx,key,128);
    for(int i=0;i<max;i++)
        aes_ecb_ttable(&ctx,AES_ENCRYPT,plain,plain);
    cout<<"final ciphertext:\n";
    for(int i=0;i<16;i++)
        printf("0x%x ",plain[i]);
    cout<<endl;
    return 0;
}

