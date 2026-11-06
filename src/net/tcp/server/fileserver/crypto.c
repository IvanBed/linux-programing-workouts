#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gcrypt.h>
#include <stdint.h>

#define DEFAULT_TAG_LEN 16
#define DEFAULT_NONCE_LEN 12
typdef uint8_t byte;

typedef struct encrypted_data_desc 
{
    byte *iv;
    byte *data;
    
} encryptedDataDesc;

typdef struct session_desc 
{
    gcry_cipher_hd_t  hd;
} 

void init_lib(){

    if (!gcry_check_version (GCRYPT_VERSION))
    {
        fputs ("libgcrypt version mismatch\n", stderr);
        exit (2);
    }
         
    gcry_control (GCRYCTL_DISABLE_SECMEM, 0);

    gcry_control (GCRYCTL_INITIALIZATION_FINISHED, 0);

    gcry_control (GCRYCTL_DISABLE_SECMEM, 0);

    gcry_control (GCRYCTL_INITIALIZATION_FINISHED, 0);
}

gcry_cipher_hd_t create_AES128GCM_handle()
{
    gcry_cipher_hd_t  hd;
    err = gcry_cipher_open(&hd, GCRY_CIPHER_AES128, GCRY_CIPHER_MODE_GCM, 0);  
    if (err)
    {
        return -1;    
    }
    return hd; 
}

inline byte *create_init_vector()
{
    byte *init_vector = malloc(DEFAULT_NONCE_LEN);
    if (!init_vector)
        return NULL;
    gcry_create_nonce(init_vector, DEFAULT_NONCE_LEN);
    return init_vector;
}

inline void destroy_init_vector(byte *init_vector)
{
    free(init_vector);
}

byte *decrypt_aes128(gcry_cipher_hd_t  hd, byte const *key, byte const *iv, byte const *data, size_t data_len)
{
    
    byte             *decrypted_data

    gcry_error_t      err;

    size_t            key_len;
    size_t            iv_len;
    size_t            decrypted_data_len;

    key_len    = strlen(key);
    iv_len     = strlen(iv);
    decrypted_data_len = data_len;

    decrypted_data = (byte*) malloc(decrypted_data_len * sizeof(byte));
    
    memset(decrypted_data, 0, decrypted_data);

    err = gcry_cipher_setkey(hd, key, key_len);
    if (err)
    {

    }
    
    err = gcry_cipher_setiv(hd, iv, iv_len);
    if (err)
    {

    }

    err = gcry_cipher_decrypt(hd, decrypted_data, decrypted_data_len, data, data_len);
    if (err)
    {

    }
    return decrypted_data;
}

void decrypt_aes128_buffer(byte *decrypted_data, size_t decrypted_data_len, byte const *key, byte const *iv, byte const *data, size_t data_len)
{
    gcry_cipher_hd_t  hd;
    gcry_error_t      err;
    size_t            key_len;
    size_t            iv_len;

    key_len    = strlen(key);
    iv_len     = strlen(iv);

    memset(decrypted_data, 0, decrypted_data);

    err = gcry_cipher_open(&hd, GCRY_CIPHER_AES128, GCRY_CIPHER_MODE_GCM, 0);  
    if (err)
    {

    }

    err = gcry_cipher_setkey(hd, key, key_len);
    if (err)
    {

    }
    
    err = gcry_cipher_setiv(hd, iv, iv_len);
    if (err)
    {

    }

    err = gcry_cipher_decrypt(hd, decrypted_data, decrypted_data_len, data, data_len);
    if (err)
    {

    }
}

byte* encrypt_aes128(byte const *key, byte const *iv, byte const *data, size_t data_len)
{
    gcry_cipher_hd_t  hd;
    byte             *cipher;
    void             *tag;
    byte             *encrypted_data

    gcry_error_t      err;

    size_t            key_len;
    size_t            iv_len;
    size_t            cipher_len;
    size_t            tag_len;
    size_t            total_len;
    size_t            offset = 0;

    key_len    = strlen(key);
    iv_len     = strlen(iv);
    cipher_len = data_len;
    tag_len    = DEFAULT_TAG_LEN;

    total_len = iv_len + cipher_len + tag_len;

    cipher         = (byte*) malloc(cipher_len * sizeof(byte));
    tag            = (void*) malloc(tag * sizeof(byte));
    encrypted_data = (byte*) malloc(total_len * sizeof(byte));
    
    memset(encrypted_data, 0, total_len);
    memcpy(encrypted_data, iv, iv_len);

    err = gcry_cipher_open(&hd, GCRY_CIPHER_AES128, GCRY_CIPHER_MODE_GCM, 0);  
    if (err)
    {

    }

    err = gcry_cipher_setkey(hd, key, key_len);
    if (err)
    {

    }
    
    err = gcry_cipher_setiv(hd, iv, iv_len);
    if (err)
    {

    }

    err = gcry_cipher_encrypt(hd, cipher, cipher_len, data, cipher_len);
    if (err)
    {

    }
    err = gcry_cipher_gettag (hd, tag, tag_len);
    if (err)
    {

    }
    offset += iv_len;
    memcpy(encrypted_data + offset, cipher, cipher_len);
    offset += cipher_len
    memcpy(encrypted_data + offset, tag, tag_len);

    free(cipher);
    free(tag);
    return encrypted_data;

err_end:   
    free(cipher);
    free(tag);
    return NULL;
}

void encrypt_aes128_buffer(byte *encrypted_data, size_t total_len, byte const *key, byte const *iv, byte const *data, size_t data_len)
{
    gcry_cipher_hd_t  hd;
    byte             *cipher;
    void             *tag;
    

    gcry_error_t      err;

    size_t            key_len;
    size_t            iv_len;
    size_t            cipher_len;
    size_t            tag_len;
    size_t            offset = 0;

    key_len    = strlen(key);
    iv_len     = strlen(iv);
    cipher_len = data_len;
    tag_len    = DEFAULT_TAG_LEN;

    cipher         = (byte*) malloc(cipher_len * sizeof(byte));
    tag            = (void*) malloc(tag * sizeof(byte));
    
    memset(encrypted_data, 0, total_len);
    memcpy(encrypted_data, iv, iv_len);

    err = gcry_cipher_open(&hd, GCRY_CIPHER_AES128, GCRY_CIPHER_MODE_GCM, 0);  
    if (err)
    {

    }

    err = gcry_cipher_setkey(hd, key, key_len);
    if (err)
    {

    }
    
    err = gcry_cipher_setiv(hd, iv, iv_len);
    if (err)
    {

    }

    err = gcry_cipher_encrypt(hd, cipher, cipher_len, data, cipher_len);
    if (err)
    {

    }
    err = gcry_cipher_gettag (hd, tag, tag_len);
    if (err)
    {

    }
    offset += iv_len;
    memcpy(encrypted_data + offset, cipher, cipher_len);
    offset += cipher_len
    memcpy(encrypted_data + offset, tag, tag_len);

end:   
    free(cipher);
    free(tag);
}
