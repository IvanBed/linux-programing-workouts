#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gcrypt.h>
#include <stdint.h>

#define DEFAULT_TAG_LEN 16
#define DEFAULT_NONCE_LEN 12
#define DEFAULT_PLAINTEXT_BUFFER 4096

typedef uint8_t byte;

typdef struct bytes
{
    byte  *bytes;
    size_t len;
} BytesHandle;

typdef struct sym_session_desc 
{
    gcry_cipher_hd_t  hd;
    byte             *key;
    size_t            key_len;
    size_t            iv_len;
    size_t            tag_len;
} SymSessionDesc;

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

SymSessionDesc *open_sym_cipher_session_fromkey(char const * key, size_t key_len, size_t nonce_len, size_t tag_len)
{
    SymSessionDesc *session = malloc(sizeof(SymSessionDesc));
    if (!session)
    {
        printf("");
        return NULL;
    }

    session->hd = create_AES128GCM_handle();
    if (session->hd == -1)
    {
        free(session);
        return NULL;
    }
    session->key     = key;
    session->key_len = key_len;
    session->iv_len  = nonce_len;
    session->tag_len = tag_len
    err = gcry_cipher_setkey(hd, key, key_len);
    if (err)
    {
        free(session);
        return NULL;
    }
    return session;
}

void close_sym_cipher_session(SymSessionDesc *session)
{
    gcry_cipher_close(session->hd);
    free(session->hd);
}

inline void create_init_vector(byte *init_vector, size_t iv_len)
{
    gcry_create_nonce(init_vector, iv_len);
}

void parse_encrypted_playload(byte *encrypted_payload, size_t encrypted_payload_len, size_t iv_len, size_t tag_len, 
                              byte **iv, byte **ciphertext, size_t *ciphertext_len, byte **tag)
{
    size_t offset = iv_len;
    *iv = encrypted_payload;
    *ciphertext = encrypted_payload + offset;
    *ciphertext_len = encrypted_payload_len - (iv_len + tag_len);
    offset += *ciphertext_len;
    *tag = encrypted_payload + offset;
}


size_t decrypt_aes128_buffer(SymSessionDesc const *cipher_session, byte const *encrypted_payload, size_t encrypted_payload_len, byte *plaintext, size_t plaintext_buff_size)
{
    byte        *ciphertext;
    byte        *tag;
    byte        *iv;

    size_t       ciphertext_len;
    size_t       plaintext_len  
    gcry_error_t err;
    
    parse_encrypted_playload(encrypted_payload, encrypted_payload_len, session->key_len, session->iv_len, session->tag_len, &iv, &ciphertext, &ciphertext_len, &tag);

    plaintext_len = ciphertext;

    if (plaintext_buff_size < plaintext_len)
    {
        return 0;
    }

    err = gcry_cipher_setiv(cipher_session->hd, iv, session->iv_len);
    if (err)
    {
        return 0;
    }

    err = gcry_cipher_decrypt(cipher_session->hd, plaintext, plaintext_len, ciphertext, ciphertext_len);
    if (err)
    {
        return 0;
    }
    return plaintext_len;
}

size_t encrypt_aes128_buffer(SymSessionDesc const *cipher_session, byte const *plaintext, size_t plaintext_len, byte *encrypted_playload, size_t encrypted_playload_buff_size)
{
    size_t            ciphertext_len;
    size_t            offset = 0;
    size_t            encrypted_playload_len;
    gcry_error_t      err;

    ciphertext_len         = plaintext_len;
    encrypted_playload_len = cipher_session->iv_len + ciphertext_len + cipher_session->tag_len;

    if (encrypted_playload_buff_size < encrypted_playload_len)
    {
        return 0;
    }

    create_init_vector(iv, cipher_session->iv_len);
    err = gcry_cipher_setiv(hd, iv, cipher_session->iv_len);
    if (err)
    {
        return 0;
    }
    memcpy(encrypted_playload, iv, cipher_session->iv_len);
    offset += cipher_session->iv_len;
    err = gcry_cipher_encrypt(cipher_session->hd, encrypted_playload + offset, ciphertext_len, plaintext, plaintext_len);
    if (err)
    {
        return 0;
    }
    offset += ciphertext_len

    err = gcry_cipher_gettag(cipher_session->hd, encrypted_playload + offset, cipher_session->tag_len);
    if (err)
    {
        return 0;
    }
    return encrypted_playload_len;
}

