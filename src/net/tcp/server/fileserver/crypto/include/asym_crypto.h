#ifndef ASYM_CRYPTO_H
#define ASYM_CRYPTO_H

#include "include/crypto_utils.h"

typdef struct asym_session_desc 
{
    gcry_sexp_t  pub_key;
    BytesHandle *pub_key_bytes;
    gcry_sexp_t  sec_key;
    gcry_sexp_t  key_spec;
    gcry_sexp_t  cipher;
    gcry_mpi_t   exp;
    size_t       nbits_data;
} AsymSessionDesc;

#endif
