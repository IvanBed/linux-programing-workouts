
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <fstream>
#include <cstdint>
#include <sstream> 
using namespace std;

static bool is_hex_char(char hex_char)
{
    if ('A' <= hex_char && hex_char <= 'F' || '0' <= hex_char && hex_char <= '9')
        return true;
    else
        return false;
}

int get_decimal_digit_from_hex_char(char hex_char)
{
    if (!is_hex_char(hex_char))
        return -1;

    switch(hex_char)
    {
        case '1':
            return 1;
        case '2':
            return 2;
        case '3':
            return 3;
        case '4':
            return 4;
        case '5':
            return 5;
        case '6':
            return 6;
        case '7':
            return 7;
        case '8':
            return 8;
        case '9':
            return 9;
        case 'A':
            return 10;
        case 'B':
            return 11;
        case 'C':
            return 12;
        case 'D':
            return 13;
        case 'E':
            return 14;
        case 'F':
            return 15;
        default:
            return 0;
    }
}

std::string bytes_to_hex(std::vector<int16_t> const &bytes)
{
    std::string hex_res = "";
    std::stringstream hex_stream;
    for (auto const & byte : bytes)
        hex_stream << std::hex << byte;
    
    return hex_stream.str();
}

int64_t hex_to_decimal_new(std::string const & hex_str)
{
    int64_t res_num = 0;
    int64_t decimal_value = 0;
    std::stringstream ss;
    for (size_t i = 1; i < hex_str; i += 2)
    {
       ss << hex_value;
       ss >> std::hex >> decimal_value;
       res_num += decimal_value;
    }
     // std::string hex_value
    
    
    return res_num;
}

void init_bytes_hash(std::vector<int16_t> const &bytes, std::vector<int16_t> &bytes_hash, size_t output_size)
{
    if (bytes.size() < output_size)
    {
        return;
    }
    for (size_t i = 0; i < output_size; i++)
    {

        bytes_hash[i] = bytes[i];
    }

}

std::vector<int16_t> simple_hash_internal(std::vector<int16_t> const &bytes, size_t output_size)
{
    std::vector<int16_t> bytes_hash(output_size, 0);                  
   
    init_bytes_hash(bytes, bytes_hash, output_size);

    int64_t j = 0;
    for (size_t i = output_size; i < bytes.size(); i++)
    {
        std::cout << "i: " << i << " j: " << j << "\n";
        
        bytes_hash[j] ^= bytes[i];
        j++;
        if (j == output_size) j = 0;
    }  

    return  bytes_hash;
}

std::vector<int16_t> simple_hash_32(std::vector<int16_t> const &bytes)
{
    return simple_hash_internal(bytes, 4);
}

int16_t hex_to_decimal(std::string const & hex_num)
{
    int16_t res_num     = 0;
    size_t  start_pos   = hex_num.size() - 1;
    for(size_t power = 0; power < hex_num.size(); power++)
    {
       res_num += (int16_t) std::pow(16, power) * get_decimal_digit_from_hex_char(toupper(hex_num[start_pos - power]));
    }
    return res_num;
}

std::vector<int16_t> hex_to_bytes(std::string const & hex_str)
{
    std::vector<int16_t> res;
    std::string hex_byte;
    for (size_t i = 1; i < hex_str.size(); i += 2)
    {
        hex_byte = hex_str.substr(i - 1, 2);
        //std::cout << "hex " << hex_byte << "\n";
        res.push_back(hex_to_decimal(hex_byte));
    }
    return res;
}

std::vector<int16_t> string_to_bytes(std::string const & str)
{
    std::vector<int16_t> res;
    for (size_t i = 0; i < str.size(); i++)
    {
        res.push_back((int16_t)str[i]);
    }
    return res;

}

void vector_concat(std::vector<int16_t> &v1, std::vector<int16_t> const & v2)
{
    v1.insert(v1.end(), v2.begin(), v2.end());
}

int main()
{
    std::string hex_str = "10fb149810a2d51234";
    std::string hex_str2 = "0xyz";
    std::vector<int16_t> str_bytes = hex_to_bytes(hex_str);
    std::vector<int16_t> str_bytes2 = string_to_bytes(hex_str2);

    for (size_t i = 0; i < str_bytes.size(); i++)
    {
        std::cout << str_bytes[i] << " ";

    }
    std::cout << "\n";
    std::vector<int16_t> res = simple_hash_32(str_bytes);
    std::string hex_res = bytes_to_hex(res);
    for (size_t i = 0; i < res.size(); i++)
   {
        std::cout << res[i] << " ";

    }
    std::cout << std::endl;
    std::cout << hex_res << "\n";
    std::cout << hex_to_decimal(hex_res) << "\n";

}
