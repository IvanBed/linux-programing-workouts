typedef uint8_t byte;

typdef struct bytes
{
    byte  *bytes;
    size_t len;
} BytesHandle;

BytesHandle * create_bytes_handle(void *data, size_t len)
{
    BytesHandle *handle = malloc(BytesHandle);
    if (!handle)
    {
        return NULL;
    }
    handle->bytes = malloc(len);
    if (!handle->bytes)
    {
        free(handle);
        return NULL;
    }

    memcpy(handle->bytes, (byte*)data, len);   
    handle->len= len;
    return handle;
}

void destroy_bytes_handle(BytesHandle *handle)
{
    free(handle->bytes);
    free(handle);
}