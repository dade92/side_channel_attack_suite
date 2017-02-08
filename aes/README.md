    AES t-table primitives, used to cipher with AES plaintext.
    
    Usage:
    declare an aes_context variable and a key, then call
        aes_setkey_enc(aes_context,key,keysize);
    tp init the variables, and 
        aes_ecb(aes_context,AES_ENCRYPT,plain_address,cipher_address)
    to encrypt plain buffer and put into cipher buffer.
