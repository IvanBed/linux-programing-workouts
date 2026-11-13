#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <fstream>
#include <cstdint>
#include <sstream> 
#include <cstring>
using namespace std;

struct bytes_block32
{
    int16_t block[4];
    size_t zero_cnt;
};


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
        //std::cout << "i: " << i << " j: " << j << "\n";
        
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

int16_t find_byte(int16_t maxbyte)
{
    /*for (int16_t byte = 1; byte < maxbyte; byte++)
    {
        if ((maxbyte ^ byte) < 256)
        {
            return byte;
        }
    }*/
    int16_t randomNum = (int16_t)(rand() % maxbyte + 1);
    return randomNum;
}

size_t nonce_optimization(std::vector<int16_t> &nonce)
{
    size_t zero_pos = 0;
    size_t max_pos = 0;
    size_t zero_cnt = 0;
    for (size_t i = 0; i < nonce.size(); i++)
    {
        if (nonce[i] == 0)
        {
            zero_cnt++;
            zero_pos = i;
        }
            
        if (nonce[i] > nonce[max_pos])
            max_pos = i;
    }
    nonce[zero_pos] = find_byte(nonce[max_pos]);
    nonce[max_pos]  = nonce[max_pos] ^ nonce[zero_pos];
    //std::cout << "zero " << nonce[zero_pos] << " max " <<  nonce[max_pos] << " find_byte " <<  find_byte(nonce[max_pos]) << std::endl;
    return zero_cnt - 1;  
}

std::vector<int16_t> find_nonce(std::vector<int16_t> &prepared_data, std::vector<int16_t> &hash)
{
    std::vector<int16_t> res;
    size_t counter = 4;
    int64_t val    = 0;
    bool has_zero  = false;
    for (size_t i = 0; i < prepared_data.size(); i++)
    {
        val = prepared_data[i] ^  hash[i];
        res.push_back(val);
        if (!val)
            has_zero = true; 
    }
    if (has_zero)
        while (nonce_optimization(res) > 0) {};
    return res;
}

int64_t total_XOR(std::vector<int16_t> &bytes)
{
    int64_t res = 1;
    for (size_t i = 0; i < bytes.size(); i++)
    {
       res ^= bytes[i];
    }
    return res;
}

std::vector<bytes_block32> make_blocks(std::vector<int16_t> &bytes)
{
    std::vector<bytes_block32> res;
    bytes_block32 cur_block;
    memset((void*)&cur_block, 0, sizeof(bytes_block32));
    for (size_t i = 0; i < bytes.size(); i++)
    {
        if (bytes[i] == 0)
        {
            cur_block.zero_cnt++;
        
        }
            
        if (i % 4 == 0 && i != 0)
        {
            res.push_back(cur_block);
            memset((void*)&cur_block, 0, sizeof(bytes_block32));
        }

        cur_block.block[i % 4] = bytes[i];
    }
    res.push_back(cur_block);
    return res;
}


void print_blocks(std::vector<bytes_block32> & blocks)
{
    for (size_t i = 0; i < blocks.size(); i++)
    {
        std::cout << " { ";
        std::cout << blocks[i].zero_cnt << "; ";
        for (size_t j = 0; j < 4; j++)
        {
            std::cout << blocks[i].block[j] << " ";
        }
        std::cout << " }";
    }

    std::cout << "\n";
}

void test1()
{
    std::string hash;
    std::string str;
    //std::string nonce = "10fb1498";
    
    std::getline(std::cin, hash);
    std::getline(std::cin, str);
    
    std::vector<int16_t> str_bytes = string_to_bytes(str);
    std::vector<int16_t> hash_bytes = hex_to_bytes(hash);
    std::vector<int16_t> prepared_data = prepare_data(str_bytes);
    std::vector<int16_t> nonce_bytes = find_nonce(prepared_data, hash_bytes);  

    std::cout << bytes_to_hex(nonce_bytes) << std::endl;
}

void test2()
{
 
    std::string str = "asfasdsdfasdfasdfasdfasdfwerqwer";
    std::string nonce = "10fb1498";
    
    std::vector<int16_t> str_bytes = string_to_bytes(str);
    std::vector<int16_t> nonce_bytes = hex_to_bytes(nonce);
    //std::vector<int16_t> prepared_data = prepare_data(str_bytes);
    //std::vector<int16_t> nonce_bytes = find_nonce(prepared_data, hash_bytes);  
    vector_concat(str_bytes, nonce_bytes, 32);
    std::vector<int16_t> hash = simple_hash_32(str_bytes);
    std::cout << str << std::endl;
     print_vector(nonce_bytes);
    std::cout << nonce << std::endl;
    std::cout << bytes_to_hex(hash) << std::endl;
}
void test3()
{
    std::string hash = "15ec19d";
    std::string str = "asfasdsdfasdfasdfasdfasdfwerqwer";


    std::vector<int16_t> str_bytes = string_to_bytes(str);
    std::vector<int16_t> hash_bytes = hex_to_bytes(hash);
    std::vector<int16_t> prepared_data = prepare_data(str_bytes);
    std::vector<int16_t> nonce_bytes = find_nonce(prepared_data, hash_bytes);  
    std::cout << str << std::endl;
    print_vector(nonce_bytes);
    std::cout << bytes_to_hex(nonce_bytes) << std::endl;
    std::cout << hash << std::endl;
    
}
int main()
{
    test1();
    /*std::string hash;
    std::string str;
    std::string nonce = "9fb03375";
    
    std::getline(std::cin, hash);
    std::getline(std::cin, str);
    
    std::vector<int16_t> str_bytes = string_to_bytes(str);
    std::vector<int16_t> hash_bytes = hex_to_bytes(hash);
    std::vector<int16_t> prepared_data = prepare_data(str_bytes);
    std::vector<int16_t> nonce_bytes = hex_to_bytes(nonce);;  
    print_vector(str_bytes);
    print_vector(nonce_bytes);
    print_vector(hash_bytes);*/
    
    //std::cout << bytes_to_hex(nonce_bytes) << std::endl;
    //print_vector(str_bytes);
    //std::vector<bytes_block32> blocks = make_blocks(str_bytes);
    //print_blocks(blocks);
    
    
}