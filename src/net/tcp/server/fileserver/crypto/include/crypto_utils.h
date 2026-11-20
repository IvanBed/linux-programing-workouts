#ifndef CRYPTO_UTILS_H
#define CRYPTO_UTILS_H

#include <gcrypt.h>

typedef uint8_t byte;

typdef struct bytes
{
    byte  *bytes;
    size_t len;
} BytesHandle;

#endif
