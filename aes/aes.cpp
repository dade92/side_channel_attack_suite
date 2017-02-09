#include"aes.hpp"

AES::AES(uint8_t*k,int ps,int mode) {
    if(ps%8!=0) {
        cout<<"Wrong plain size."<<endl;
        exit(0);
    }
    plainSize=ps/8;
    key=new uint8_t[plainSize];
    for(int i=0;i<plainSize;i++)
        key[i]=k[i];
    if(mode==AES_ENCRYPT)
        aes_setkey_enc(&ctx,key,ps);
    else if(mode==AES_DECRYPT)
        aes_setkey_dec(&ctx,key,ps);
    else {
        cout<<"Wrong mode."<<endl;
        exit(0);
    }
}

void AES::encrypt(uint8_t*plain,uint8_t*cipher) {
    aes_ecb_ttable(&ctx,AES_ENCRYPT,plain,cipher);
}

void AES::decrypt(uint8_t*cipher,uint8_t*plain) {
    aes_ecb_ttable(&ctx,AES_DECRYPT,cipher,plain);
}