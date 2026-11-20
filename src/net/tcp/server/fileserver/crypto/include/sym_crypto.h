#ifndef ASYM_CRYPTO_H
#define ASYM_CRYPTO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "include/crypto_utils.h"

#define DEFAULT_TAG_LEN 16
#define DEFAULT_NONCE_LEN 12
#define DEFAULT_PLAINTEXT_BUFFER 4096

typdef struct sym_session_desc 
{
    gcry_cipher_hd_t  hd;
    byte             *key;
    size_t            key_len;
    size_t            iv_len;
    size_t            tag_len;
} SymSessionDesc;

#endif

