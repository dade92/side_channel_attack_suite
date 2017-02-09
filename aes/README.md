    AES t-table primitives, used to cipher with AES plaintext.
    
    Usage:
    declare an aes_context variable and a key, then call
    
    aes_setkey_enc(aes_context,key,keysize);
    
    tp init the variables, and 
    
    aes_ecb(aes_context,AES_ENCRYPT,plain_address,cipher_address)
    
    to encrypt plain buffer and put into cipher buffer.
    
    Or you can use the simpler wrapper class AES that manages
    some dirty stuff, without calling those functions.
    Usage:
    AES aes(key,bit_plain_size,mode);
    aes.encrypt(plain,cipher); // aes.decrypt(cipher,plain);
