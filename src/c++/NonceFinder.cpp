#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <fstream>
#include <cstdint>
#include <sstream> 
#include <cstring>
 
using namespace std;

enum block_state 
{
    NO_PADDING,
    ONE_ZERO_PADDING,
    TWO_ZEROS_PADDING,
    THREE_ZEROS_PADDING,
};

struct bytes_block32
{
    int16_t block[4];
};

std::string bytes_to_hex(std::vector<int16_t> const &bytes)
{
    std::string hex_res = "";
    std::stringstream hex_stream;
    for (auto const & byte : bytes)
    {
        if (byte < 16)
            hex_stream << "0" << std::hex << byte;     
        else 
            hex_stream << std::hex << byte;
    }
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

std::vector<int16_t> string_to_bytes(std::string const & str)
{
    //std::vector<std::byte> stdbytes = to_bytes(str);
    std::vector<int16_t> res;
    for (size_t i = 0; i < str.size(); i++)
    {
        res.push_back((int16_t)str[i]);
    }
    return res;
    
}

class Blocks32Desc 
{
    private:
    std::vector<bytes_block32> blocks;
    block_state state;
    size_t el_cnt;
    void make_blocks(std::vector<int16_t> const &bytes)
    {
        bytes_block32 cur_block;
        memset((void*)&cur_block, 0, sizeof(bytes_block32));
        for (size_t i = 0; i < bytes.size(); i++)
        {
            if (i % 4 == 0 && i != 0)
            {
                blocks.push_back(cur_block);
                memset((void*)&cur_block, 0, sizeof(bytes_block32));
            }

            cur_block.block[i % 4] = bytes[i];
        }
        blocks.push_back(cur_block);
    }

    void set_block_state_internal()
    {
        if (blocks.size() == 0)
            return;

        bytes_block32 last_block = blocks[blocks.size() - 1];
        size_t zeros_cnt = el_cnt % 4;
        switch(zeros_cnt) 
        {
            case 0:
               state = NO_PADDING;
               break;
            case 3:
               state = ONE_ZERO_PADDING;
               break;             
            case 2:
               state = TWO_ZEROS_PADDING;
               break; 
            case 1:
               state = THREE_ZEROS_PADDING;
               break;                
        }
    }
    
    public:
    Blocks32Desc(std::vector<int16_t> const & bytes) 
    {
        el_cnt = bytes.size();
        make_blocks(bytes);            
        set_block_state_internal();
    }

    std::vector<bytes_block32> & get_blocks()
    {
        return blocks;
    }

    size_t get_size()
    {
        return blocks.size();
    }
    
    
    bytes_block32 *get_first_block()
    {
        if (blocks.size() == 0)
            return nullptr;

        return &blocks[0];
    }

    bytes_block32 *get_last_block()
    {
        if (blocks.size() == 0)
            return nullptr;

        return &blocks[blocks.size() - 1];
    }

    bytes_block32 *get_middle_block()
    {
        if (blocks.size() < 2)
            return nullptr;

        return &blocks[blocks.size() - 2];
    }

    block_state get_state()
    {
        return state;
    }

    void delete_middle_blocks()
    {
        blocks.erase(blocks.begin() + 1, blocks.end() - 1); 
    }

    void add_block(std::vector<int16_t> const &bytes)
    {
        bytes_block32 cur_block;
        memset((void*)&cur_block, 0, sizeof(bytes_block32));
        for (size_t i =0; i < 4; i++)
        {
            cur_block.block[i] = bytes[i];
        }
        blocks.push_back(cur_block);
    }

    void print_blocks()
    {
        for (size_t i = 0; i < blocks.size(); i++)
        {
            std::cout << " { ";
            for (size_t j = 0; j < 4; j++)
            {
                std::cout << blocks[i].block[j] << " ";
            }
            std::cout << " }";
        }
        std::cout << "\n";
        
    }

    void print_state() 
    {
        switch(state) 
        {
            case NO_PADDING:
                std::cout << "NO_PADDING\n";
                break;
            case ONE_ZERO_PADDING:
                std::cout << "ONE_ZERO_PADDING\n";
                break;             
            case TWO_ZEROS_PADDING:
                std::cout << "TWO_ZEROS_PADDING\n";
                break; 
            case THREE_ZEROS_PADDING:
                std::cout << "THREE_ZEROS_PADDING\n";
                break;
            default:
                std::cout << "err\n";
        }
    }
};

class NonceFinder 
{
    private:
    Blocks32Desc         *data_blocks;
    std::vector<int16_t>  hash;
    std::vector<int16_t>  fake_nonce;
    std::vector<int16_t>  nonce;
    std::string           nonce_hex;
    void mark_nonce_pos()
    {
        bytes_block32 *last_block = data_blocks->get_last_block();

        size_t start_index = 0;
        size_t nonce_index = 0;
        switch(data_blocks->get_state())
        {
            case NO_PADDING:
            return;
            case ONE_ZERO_PADDING:
            start_index = 3;
            break;
            case TWO_ZEROS_PADDING:
            start_index = 2;
            break;
            case THREE_ZEROS_PADDING:
            start_index = 1;
            break;           
        }

        while (start_index < 4)
        {
            last_block->block[start_index] = 0;
            fake_nonce[nonce_index] = 0;
            start_index++;
            nonce_index++;
        }
    }

    void precalc_hash() 
    {
        if (data_blocks->get_size() < 2)
        {
            return;
        }
        bytes_block32 *first_block = data_blocks->get_first_block();   
        bytes_block32 *last_block = data_blocks->get_last_block();        
        for (size_t i = 1; i < data_blocks->get_size() - 1; i++)
        {
            bytes_block32 *cur_block = &data_blocks->get_blocks()[i];
            for (size_t j = 0; j < 4; j++)
            {
                first_block->block[j] ^= cur_block->block[j];
            }
        }
        data_blocks->delete_middle_blocks(); 
    }
    void prepare_data()
    {
        mark_nonce_pos();
        precalc_hash();
        data_blocks->add_block(fake_nonce);
    }

    std::pair<int16_t, int16_t> bruteforce_pair(int16_t data_byte, int16_t hash_byte)
    {
        for (int16_t first_nonce_byte = 0; first_nonce_byte < 256; first_nonce_byte++)
        {
            for (int16_t second_nonce_byte = 0; second_nonce_byte < 256; second_nonce_byte++)
            {    
            
                if ((data_byte ^ first_nonce_byte ^ second_nonce_byte) == hash_byte)
                {
                    return {first_nonce_byte, second_nonce_byte};
                }
            }
        }
        return {0, 0};
    }

    void find_nonce_internal()
    {
        bytes_block32 *first_block;
        bytes_block32 *middle_block;
        bytes_block32 *last_block = data_blocks->get_last_block(); 
        if (data_blocks->get_size() >= 3)
        {
            first_block = data_blocks->get_first_block();
            middle_block = data_blocks->get_middle_block();  
        } 
        else 
        {
            first_block = data_blocks->get_first_block();
            bytes_block32 middle_block_temp;
            memset((void*)&middle_block_temp, 0, sizeof(bytes_block32));
            middle_block = &middle_block_temp;
        }
              
        std::pair<int16_t, int16_t> nonce_pair;
        std::vector<int16_t> true_nonce(4, 0);

        if (data_blocks->get_state() == ONE_ZERO_PADDING)
        {
            nonce_pair    = bruteforce_pair(first_block->block[3], hash[3]);
            true_nonce[0] = nonce_pair.first;
            true_nonce[1] = first_block->block[1] ^ middle_block->block[1] ^ hash[1];
            true_nonce[2] = first_block->block[2] ^ middle_block->block[2] ^ hash[2];
            true_nonce[3] = nonce_pair.second;

        } 
        else if (data_blocks->get_state() == TWO_ZEROS_PADDING)
        {
            std::pair<int16_t, int16_t> nonce_pair_two;
            nonce_pair     = bruteforce_pair(first_block->block[2], hash[2]);
            nonce_pair_two = bruteforce_pair(first_block->block[3], hash[3]);
            true_nonce[0]  = nonce_pair.first;
            true_nonce[1]  = nonce_pair_two.first;
            true_nonce[2]  = nonce_pair.second;
            true_nonce[3]  = nonce_pair_two.second;
        }  
        else if (data_blocks->get_state() == THREE_ZEROS_PADDING) 
        {
            nonce_pair    = bruteforce_pair(first_block->block[3], hash[3]);
            true_nonce[0] = first_block->block[1] ^ hash[1];
            true_nonce[1] = first_block->block[2] ^ hash[2];
            true_nonce[2] = nonce_pair.first;
            true_nonce[3] = nonce_pair.second;
        } 
        else 
        {
            for (size_t i = 0; i < 4; i++)
            {
                true_nonce[i] = hash[i] ^ first_block->block[i] ^ middle_block->block[i];
            }
        }
        nonce = true_nonce;
   
        nonce_hex = bytes_to_hex(nonce);
    }

    public:
    NonceFinder(std::vector<int16_t> const & data_bytes, std::vector<int16_t> const & hash)
    {
        std::vector<int16_t> temp(4, -1);
        fake_nonce = temp;
        data_blocks = new Blocks32Desc(data_bytes);
        
        this->hash = hash;
    }
    
    ~NonceFinder()
    {
        delete data_blocks;
    }

    std::string const & get_nonce_hex()
    {
        return nonce_hex;
    }

    std::vector<int16_t>  const & get_nonce()
    {
        return nonce;
    }

    void find_nonce() 
    {
        prepare_data();  
        find_nonce_internal();
    }
};

void solve()
{
    std::string hash;
    std::string str;
    std::string nonce_hex;
    
    std::getline(std::cin, hash);
    std::getline(std::cin, str);
    std::vector<int16_t> str_bytes = string_to_bytes(str);
    std::vector<int16_t> hash_bytes = hex_to_bytes(hash);; 
   
    NonceFinder *nonce_finder = new NonceFinder(str_bytes, hash_bytes);
    nonce_finder->find_nonce(); 
    nonce_hex = nonce_finder->get_nonce_hex();
    std::cout << nonce_hex << "\n";
    delete nonce_finder;
}


int main()
{
    solve();
}
