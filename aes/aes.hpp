#ifndef AES_H_
#define AES_H_
#include<iostream>
#include<cstdint>

#include"aes_ttable.hpp"

using namespace std;

/**
 * Class that execute an
 * AES encryption
 * or decryption
 */

enum mode {
    encrypt,decrypt
};

class AES {
public:
    AES(uint8_t*key,int plainSize,int mode);
    void encrypt(uint8_t*plain,uint8_t* cipher);
    void decrypt(uint8_t*cipher,uint8_t* plain);
private:
    uint8_t*key;
    int plainSize;
    aes_context ctx;
    int mode;
};
#endif