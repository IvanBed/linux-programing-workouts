#include <gcrypt.h>


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

AsymSessionDesc *open_asym_session(size_t nbits_data)
{
    int err;
    AsymSessionDesc *session = malloc(sizeof(AsymSessionDesc));
    if (!session)
    {
        printf("");
        return NULL;
    }
    memset(session, 0, sizeof(AsymSessionDesc));
    err = gcry_sexp_new(&(session->key_spec), "(genkey (rsa (nbits 4:2048)))", 0, 1);
    if (err) 
    {
        
    }
    session->nbits_data;
    session->exp = gcry_mpi_new(nbits_data);
    gcry_mpi_randomize(session->exp, nbits_data, GCRY_WEAK_RANDOM);
    return session;
}

void close_asym_session(AsymSessionDesc *session)
{ 
    gcry_sexp_release(session->key_spec);
    //gcry_ac_key_destroy(session->key_pair);
    if(pub_key_bytes)
        free(session->pub_key_bytes);
    free(session);

}

void gen_RSA_keys(AsymSessionDesc *session)
{
    int err;
    gcry_sexp_t key;
    
    err = gcry_pk_genkey(&key, key_spec);
     
    if (err) 
    {
        
    }

    session->pub_key = gcry_sexp_find_token(key, "public-key", 0);
    if (!session->pub_key) 
    {
        
    }

    session->sec_key = gcry_sexp_find_token(key, "private-key", 0);
    if (!session->sec_key) 
    {

    }
}

gcry_sexp_t prepare_plaintext(AsymSessionDesc *session, byte *plaintext)
{
    int err;
    gcry_sexp_t plaintext_sexp;
    err = gcry_sexp_build(&plaintext_sexp, NULL, "(data %s (flags raw) (value %m))", (char*) plaintext, x);
    if (err)
    {
        
    }
    return plaintext_sexp;
}

BytesHandle *asym_encrypt(AsymSessionDesc *session, BytesHandle const *plaintext)
{
    int err;
    gcry_sexp_t cipher_sexp, plaintext_sexp;
    BytesHandle *ciphertext;

    plaintext_sexp = prepare_plaintext(session, plaintext->bytes);

    err = gcry_pk_encrypt(&cipher_sexp, plaintext_sexp, session->pub_key);
    if (err) 
    {
        
    }
    gcry_sexp_t l = gcry_sexp_find_token(cipher_sexp, "a", 0);

    char const *data = gcry_sexp_nth_data(l, 1, &len);
    
    ciphertext = create_bytes_handle(data, len);
    if (!ciphertext)
    {


    }
    return ciphertext;
}