#include "base64_to_bytes.hpp"

void build_decoding_table() 
{

    decoding_table = (char *) malloc(256);

    for (int i = 0; i < 64; i++)
        decoding_table[(unsigned char) encoding_table[i]] = i;
}

void base64_cleanup() 
{
    free(decoding_table);
}

std::string base64_decode_to_string(std::string const & str) 
{
    std::string decoded_data = "";
    char const *data;
    size_t input_length;
    size_t output_length;
    
    data = str.c_str(); 
    input_length = str.size();

    if (decoding_table == NULL) 
        build_decoding_table();

    if (input_length % 4 != 0) 
        return decoded_data;

    output_length = input_length / 4 * 3;
    if (data[input_length - 1] == '=') 
        (output_length)--;
    if (data[input_length - 2] == '=') 
        (output_length)--;

    decoded_data.reserve(output_length);

    for (int i = 0, j = 0; i < input_length;) 
    {

        uint32_t sextet_a = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_b = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_c = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_d = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];

        uint32_t triple = (sextet_a << 3 * 6) + (sextet_b << 2 * 6) + (sextet_c << 1 * 6) + (sextet_d << 0 * 6);

        if (j++ < output_length) 
            decoded_data.push_back((triple >> 2 * 8) & 0xFF);
        if (j++ < output_length) 
            decoded_data.push_back((triple >> 1 * 8) & 0xFF);
        if (j++ < output_length) 
            decoded_data.push_back((triple >> 0 * 8) & 0xFF);
    }

    return decoded_data;
}

std::vector<uint8_t> base64_decode_to_bytes(std::string const & str) 
{
    std::vector<uint8_t> empty;
    char const *data;
    size_t input_length;
    size_t output_length;
    
    data = str.c_str(); 
    input_length = str.size();

    if (decoding_table == NULL) 
        build_decoding_table();

    if (input_length % 4 != 0) 
    {
        std::cout << "NOT DISISIABLE ON 4";
        return empty;
    }
        

    output_length = input_length / 4 * 3;
    if (data[input_length - 1] == '=') 
        (output_length)--;
    if (data[input_length - 2] == '=') 
        (output_length)--;

    std::vector<uint8_t> decoded_data(output_length, 0);

    for (int i = 0, j = 0; i < input_length;) 
    {
        uint32_t sextet_a = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_b = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_c = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_d = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];

        uint32_t triple = (sextet_a << 3 * 6) + (sextet_b << 2 * 6) + (sextet_c << 1 * 6) + (sextet_d << 0 * 6);

        if (j < output_length) 
            decoded_data[j++] = (triple >> 2 * 8) & 0xFF;
        if (j < output_length) 
            decoded_data[j++] = (triple >> 1 * 8) & 0xFF;
        if (j < output_length) 
            decoded_data[j++] = (triple >> 0 * 8) & 0xFF;
    }

    return decoded_data;
}
