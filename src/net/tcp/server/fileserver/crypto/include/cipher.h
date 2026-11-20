#ifndef CIPHER_H
#define CIPHER_H

#include "include/asym_crypto.h"
#include "include/sym_crypto.h"

typedef struct cipher_desc
{
    AsymSessionDesc *asyn_session;
    SymSessionDesc *syn_session;

} CipeherDesc;

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

#endif