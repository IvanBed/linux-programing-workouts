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

void print_vector(std::vector<int16_t> &v)
{
    for (size_t i = 0; i < v.size(); i++)
    {
        std::cout << v[i] << " ";
    }
    std::cout << "\n";
}

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

std::vector<std::byte> to_bytes(std::string const& s)
{
    std::vector<std::byte> bytes;
    bytes.reserve(std::size(s));
      
    std::transform(std::begin(s), std::end(s), std::back_inserter(bytes), [](char c){
        return std::byte(c);
    });

    return bytes;
}

std::vector<int16_t> string_to_bytes(std::string const & str)
{
    std::vector<std::byte> stdbytes = to_bytes(str);
    std::vector<int16_t> res;
    for (size_t i = 0; i < stdbytes.size(); i++)
    {
        res.push_back((int16_t)stdbytes[i]);
    }
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


class Blocks32Desc 
{
    private:
    std::vector<bytes_block32> blocks;
    block_state state;
    size_t el_cnt;
    void make_blocks(std::vector<int16_t> &bytes)
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
        //for (size_t i = 0; i < 4; i++)
        //{
        //    if (last_block.block[i] == 0) zeros_cnt++;
        //}

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
    Blocks32Desc(std::vector<int16_t> &bytes) 
    {
        el_cnt = bytes.size();
        make_blocks(bytes);            
        set_block_state_internal();
    }

    std::vector<bytes_block32> & get_blocks()
    {
        return blocks;
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

    block_state reset_block_state()
    {
        set_block_state_internal();
        return get_state();
    }

    void delete_middle_blocks()
    {
        blocks.erase(blocks.begin() + 1, blocks.end() - 1); 
    }

    void add_block(std::vector<int16_t> &bytes)
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

class Hash32Desc 
{
    private:
    Blocks32Desc        *data_blocks;
    std::vector<int16_t> nonce;
    std::vector<int16_t> base_nonce;
    
    std::vector<int16_t> hash;

    void make_padding()
    {
        if (!data_blocks)
            return;

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
            last_block->block[start_index] = nonce[nonce_index];
            nonce[nonce_index] = 0;
            start_index++;
            nonce_index++;
        }
        //data_blocks->reset_block_state();
    }

    void add_nonce()
    {
        data_blocks->add_block(nonce);
    }

    public:
    Hash32Desc(std::vector<int16_t> &data_bytes, std::vector<int16_t> &nonce_bytes) 
    {
        data_blocks = new Blocks32Desc(data_bytes);
        nonce       = nonce_bytes;
        base_nonce  = nonce_bytes;
    }

    void generate_hash()
    {   
        make_padding();
        add_nonce();  

        bytes_block32 *first_block = data_blocks->get_first_block();          
        for (size_t i = 0; i < 4; i++)
        {
            hash.push_back(first_block->block[i]);
        }
        for (size_t i = 1; i < data_blocks->get_blocks().size(); i++)
        {
            bytes_block32 *cur_block = &data_blocks->get_blocks()[i];
            for (size_t j = 0; j < 4; j++)
            {
                hash[j] ^= cur_block->block[j];
            }
        }
    }

    std::vector<int16_t> &get_hash()
    {
        return hash;
    }

    ~Hash32Desc()
    {
        delete data_blocks;
    }

    void print_nonce()
    {
        std::cout << "work nonce\n";
        for (size_t i = 0; i < nonce.size(); i++)
        {
            std::cout << nonce[i]<< " ";     
        }
        std::cout << "\nbase nonce\n";
        for (size_t i = 0; i < base_nonce.size(); i++)
        {
            std::cout << base_nonce[i]<< " ";     
        }
        std::cout << "\n";
    }

    void print_data()
    {
        data_blocks->print_blocks();
         
        data_blocks->print_state();
    }
    
    void print_hash()
    {
        for (size_t i =0; i < hash.size(); i++)
            std::cout << hash[i] << " ";
         
        std::cout << "\n";    
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
            last_block->block[start_index] = -1;
            fake_nonce[nonce_index] = 0;
            start_index++;
            nonce_index++;
        }
    }

    void precalc_hash() 
    {
        bytes_block32 *first_block = data_blocks->get_first_block();   
        bytes_block32 *last_block = data_blocks->get_last_block();        
        for (size_t i = 1; i < data_blocks->get_blocks().size() - 1; i++)
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

    int16_t bruteforce(int16_t data_byte, int16_t hash_byte)
    {
        for (int16_t nonce_byte = 16; nonce_byte < 256; nonce_byte++)
        {
            if ((data_byte ^ nonce_byte) == hash_byte)
            {
                return nonce_byte;
            }
        }
        return -1;
    }

    std::pair<int16_t, int16_t> bruteforce_pair(int16_t data_byte, int16_t hash_byte)
    {
        for (int16_t first_nonce_byte = 159; first_nonce_byte < 256; first_nonce_byte++)
        {
            for (int16_t second_nonce_byte = 1; second_nonce_byte < 256; second_nonce_byte++)
            {    
            
                if ((data_byte ^ first_nonce_byte ^ second_nonce_byte) == hash_byte)
                {
                    return {first_nonce_byte, second_nonce_byte};
                }
            }
        }
        return {-1, -1};
    }

    void print_block(bytes_block32 * block)
    {
        for (size_t i = 0; i < 4; i++)
        {
            std::cout << block->block[i] << " ";
        }        
        std::cout << "\n";
    }

    void find_nonce_internal()
    {
        bytes_block32 *first_block = data_blocks->get_first_block();
        bytes_block32 *middle_block = data_blocks->get_middle_block();        
        bytes_block32 *last_block = data_blocks->get_last_block(); 
        
        //print_block(first_block);
        //print_block(middle_block);
        //print_block(last_block);
        
        std::pair<int16_t, int16_t> nonce_pair;
        std::vector<int16_t> true_nonce(4, 0);

        if (data_blocks->get_state() == ONE_ZERO_PADDING)
        {
            nonce_pair    = bruteforce_pair(first_block->block[3], hash[3]);
            true_nonce[0] = nonce_pair.first;
            true_nonce[1] = bruteforce(first_block->block[1] ^ middle_block->block[1], hash[1]);
            true_nonce[2] = bruteforce(first_block->block[2] ^ middle_block->block[2], hash[2]);
            true_nonce[3] = nonce_pair.second;

        } else if (data_blocks->get_state() == TWO_ZEROS_PADDING)
        {
            std::pair<int16_t, int16_t> nonce_pair_two;
            nonce_pair     = bruteforce_pair(first_block->block[2], hash[2]);
            nonce_pair_two = bruteforce_pair(first_block->block[3], hash[3]);
            true_nonce[0]  = nonce_pair.first;
            true_nonce[1]  = nonce_pair_two.first;
            true_nonce[2]  = nonce_pair.second;
            true_nonce[3]  = nonce_pair_two.second;
        }  else if (data_blocks->get_state() == THREE_ZEROS_PADDING) 
        {
            true_nonce[0] = bruteforce(first_block->block[1], hash[1]);
            true_nonce[1] = bruteforce(first_block->block[2], hash[2]);
            nonce_pair    = bruteforce_pair(first_block->block[3], hash[3]);
            true_nonce[2] = nonce_pair.first;
            true_nonce[3] = nonce_pair.second;

        } else 
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
    NonceFinder(std::vector<int16_t> data_bytes, std::vector<int16_t> hash)
    {
        std::vector<int16_t> temp(4, -1);
        fake_nonce = temp;
        data_blocks = new Blocks32Desc(data_bytes);
        print_data();
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

    void print_data()
    {
        data_blocks->print_blocks();
        data_blocks->print_state();
    }
    void print_nonce()
    {
        for (size_t i = 0; i < nonce.size(); i++)
        {
            std::cout << nonce[i] << " ";
        }
        std::cout << "\n";
    }
    
    void print_nonce_hex()
    {
        std::string nonce_hex = bytes_to_hex(nonce);
        for (size_t i = 0; i < nonce_hex.size(); i++)
        {
            std::cout << nonce_hex[i] << "";
        }
        std::cout << "\n";
        //std::cout << total_XOR(nonce) << "\n";
    }    
};

void test1()
{
    //45a060c5   43852d88
    std::string str = "йцукенгшщзхждлорпав";
    std::string nonce = "1ab301cc";
    std::vector<int16_t> str_bytes = string_to_bytes(str);
    std::vector<int16_t> nonce_bytes = hex_to_bytes(nonce);; 
    print_vector(str_bytes); 
    int64_t nonce_xor = total_XOR(nonce_bytes); 
    Hash32Desc *hash_desc = new Hash32Desc(str_bytes, nonce_bytes);
    hash_desc->generate_hash();

    std::string hash_str = bytes_to_hex(hash_desc->get_hash());
    std::cout << "HASH: ";
    for (size_t i = 0; i < hash_str.size(); i++)
    {
        std::cout << hash_str[i];
    }
    std::cout << "\n";
    std::cout << "nonce: " << nonce << "\n";
    std::cout << "nonce_xor: " << nonce_xor << "\n";

    delete hash_desc;
}

void test2()
{
    std::string str = "йцукенгшщзхждлорпав";
    std::string hash = "0137cafc";
    
    std::vector<int16_t> str_bytes = string_to_bytes(str);
    std::vector<int16_t> hash_bytes = hex_to_bytes(hash);; 
    std::cout << "hash bytes\n";
    print_vector(hash_bytes);
    NonceFinder *nonce_finder = new NonceFinder(str_bytes, hash_bytes);
    nonce_finder->find_nonce(); 
    std::cout << "------------------------------------------------------------------------\n";
    std::cout << "NONCE: ";
    nonce_finder->print_nonce_hex();
    delete nonce_finder;
}

void test3()
{
    std::string str = "dfgsdfgsdfgsdfgsdfgsdgdgsdggga";
    std::string hash = "014466ad";
     

    std::vector<int16_t> str_bytes = string_to_bytes(str);
    std::vector<int16_t> hash_bytes = hex_to_bytes(hash);; 
   
    NonceFinder *nonce_finder = new NonceFinder(str_bytes, hash_bytes);
    nonce_finder->find_nonce(); 
    nonce_finder->print_nonce_hex();
    delete nonce_finder;
}

void solve()
{
    std::string hash;
    std::string str;
     
    
    std::getline(std::cin, hash);
    std::getline(std::cin, str);
    std::vector<int16_t> str_bytes = string_to_bytes(str);
    std::vector<int16_t> hash_bytes = hex_to_bytes(hash);; 
   
    NonceFinder *nonce_finder = new NonceFinder(str_bytes, hash_bytes);
    nonce_finder->find_nonce(); 
    nonce_finder->print_nonce_hex();
    delete nonce_finder;

}


int main()
{
    test2();
    
}