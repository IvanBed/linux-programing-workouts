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


int64_t hex_to_decimal(std::string const & hex_num)
{
    int64_t res_num     = 0;
    size_t  start_pos   = hex_num.size() - 1;
    for(size_t power = 0; power < hex_num.size(); power++)
    {
       res_num += (int64_t) std::pow(16, power) * get_decimal_digit_from_hex_char(toupper(hex_num[start_pos - power]));
    }
    return res_num;
}

std::string bytes_to_hex(std::vector<int16_t> const &bytes)
{
    std::string hex_res = "";
    std::stringstream hex_stream;
    for (auto const & byte : bytes)
        hex_stream << std::hex << byte;
    
    return hex_stream.str();
}

std::vector<int16_t> hex_to_bytes(std::string const & hex_str)
{
    std::vector<int16_t> res;
    
    std::stringstream ss;
    for (size_t i = 1; i < hex_str.size(); i += 2)
    {
       res.push_back(stoul(hex_str.substr(i - 1, 2), 0, 16));
    }
 
    return res;
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


std::vector<int16_t> string_to_bytes(std::string const & str)
{
    std::vector<int16_t> res;
    for (size_t i = 0; i < str.size(); i++)
    {
        res.push_back((int16_t)str[i]);
    }
    return res;
    
}

void alignment(std::vector<int16_t> &bytes, size_t target_size)
{
    for (size_t i = 0; i < target_size - bytes.size(); i++)
    {
        bytes.push_back(0);
    }
}

void vector_concat(std::vector<int16_t> &v1, std::vector<int16_t> & v2,  size_t target_size)
{
    for(size_t i = 0; i < target_size - v1.size(); i++)
    {
        v1.push_back(v2[i]);
        v2[i] = 0;
    }
    v1.insert(v1.end(), v2.begin(), v2.end());
}

void print_vector(std::vector<int16_t> &v)
{
    for (size_t i = 0; i < v.size(); i++)
    {
        std::cout << v[i] << " ";
    }
    std::cout << "\n";
}

std::vector<int16_t> prepare_data(std::vector<int16_t> &bytes)
{
    std::vector<int16_t> res = simple_hash_32(bytes);
    return res;
}

int16_t bruteforce(int16_t data_byte, int16_t hash_byte)
{
    for (int16_t nonce_byte = 0; nonce_byte < 256; nonce_byte++)
    {
        if ((data_byte ^ nonce_byte) == hash_byte)
        {
            return nonce_byte;
        }
    }
    return -1;
}

std::vector<int16_t> find_nonce(std::vector<int16_t> &prepared_data, std::vector<int16_t> &hash)
{
    std::vector<int16_t> res;
    size_t counter = 4;
    int16_t val    = 0;
    for (size_t i = 0; i < prepared_data.size(); i++)
    {
        val = bruteforce(prepared_data[i], hash[i]);
        if (val == 0)
           counter++;
        else if (val == -1)
        {
            while(counter > 0 || )
        } 
        res.push_back();
    }
    return res;
}

int main()
{
    std::string hash = "45a060c5";
    std::string str = "1234diGi";
    std::string nonce = "10fb1498";
    /*std::vector<int16_t> str_bytes = string_to_bytes(str);
    std::vector<int16_t> nonce_bytes = hex_to_bytes(nonce);
    for (size_t i = 0; i < str_bytes.size(); i++)
    {
        std::cout << str_bytes[i] << " ";

    }
    std::cout << std::endl;
    vector_concat(str_bytes, nonce_bytes, 8);
    std::vector<int16_t> res = simple_hash_32(str_bytes);
    print_vector(str_bytes);
    for (size_t i = 0; i < res.size(); i++)
    {
        std::cout << res[i] << " ";

    }
    std::cout << bytes_to_hex(res) << std::endl;*/
    std::vector<int16_t> str_bytes = string_to_bytes(str);
    std::vector<int16_t> hash_bytes = hex_to_bytes(hash);
    std::vector<int16_t> prepared_data = prepare_data(str_bytes);
    std::vector<int16_t> nonce_bytes = find_nonce(prepared_data, hash_bytes);  
    for (size_t i = 0; i < nonce_bytes.size(); i++)
    {
        std::cout << nonce_bytes[i] << " ";

    }
    std::cout << std::endl << bytes_to_hex(res) << std::endl;
}