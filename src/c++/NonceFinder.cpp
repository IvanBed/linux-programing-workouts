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
    ONE_ZEROS_PADDING,
    TWO_ZEROS_PADDING,
    THREE_ZEROS_PADDING,
};

struct bytes_block32
{
    int16_t block[4];

};

class Blocks32Desc 
{
    private:
    std::vector<bytes_block32> blocks;
    block_state state;
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
        size_t zeros_cnt = 0;
        for (size_t i = 0; i < 4; i++)
        {
            if (last_block.block[i] == 0) zeros_cnt++;
        }

        switch(zeros_cnt) 
        {
            case 0:
               state = NO_PADDING;
               break;
            case 1:
               state = ONE_ZEROS_PADDING;
               break;             
            case 2:
               state = TWO_ZEROS_PADDING;
               break; 
            case 3:
               state = THREE_ZEROS_PADDING;
               break;                
        }
    }
    
    public:
    Blocks32Desc(std::vector<int16_t> &bytes) 
    {
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
            case ONE_ZEROS_PADDING:
                std::cout << "ONE_ZEROS_PADDING\n";
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

    void padding()
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
            case ONE_ZEROS_PADDING:
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
        data_blocks->reset_block_state();
    }
    
    public:
    Hash32Desc(std::vector<int16_t> &data_bytes, std::vector<int16_t> &nonce_bytes) 
    {
        data_blocks = new Blocks32Desc(data_bytes);
        nonce = nonce_bytes;
        base_nonce = nonce_bytes;
    }

    void add_nonce()
    {
        data_blocks->add_block(nonce);
    }

    void make_padding()
    {
        padding();
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

    void mark_nonce_pos()
    {
        bytes_block32 *last_block = data_blocks->get_last_block();

        size_t start_index = 0;
        size_t nonce_index = 0;
        switch(data_blocks->get_state())
        {
            case NO_PADDING:
            return;
            case ONE_ZEROS_PADDING:
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

    public:
    NonceFinder(std::vector<int16_t> &data_bytes, std::vector<int16_t> &hash)
    {
        std::vector<int16_t> temp(4, -1);
        fake_nonce = temp;
        data_blocks = new Blocks32Desc(data_bytes);
        print_data();
        hash = hash;
    }
    
    ~NonceFinder()
    {

    }

    void print_data()
    {
        data_blocks->print_blocks();
        data_blocks->print_state();
    }

    void prepare_data()
    {
        mark_nonce_pos();
        precalc_hash();
        data_blocks->add_block(fake_nonce);
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

    void find_nonce()
    {
        
        std::vector<int16_t>  true_nonce(4, 0);

        if (data_blocks->state == ONE_ZERO_PADDING)
        {

        } else if (data_blocks->state == TWO_ZEROS_PADDING)
        {
            
        }  else if (data_blocks->state == THREE_ZEROS_PADDING) 
        {

        } else 
        {
            
        }
    }

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

std::vector<int16_t> string_to_bytes(std::string const & str)
{
    std::vector<int16_t> res;
    for (size_t i = 0; i < str.size(); i++)
    {
        res.push_back((int16_t)str[i]);
    }
    return res;
    
}

void print_vector(std::vector<int16_t> &v)
{
    for (size_t i = 0; i < v.size(); i++)
    {
        std::cout << v[i] << " ";
    }
    std::cout << "\n";
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

void test4()
{
    std::string hash = "15ec19d";
    std::string str = "asfasdsdfasdfasdfasdfasdfwerasa";
    std::vector<int16_t> str_bytes = string_to_bytes(str);
    std::vector<int16_t> hash_bytes = hex_to_bytes(hash);
    Blocks32Desc *bd = new Blocks32Desc(str_bytes);
     
    bd->print_blocks();
    bd->print_state();
}

void test5()
{
    //45a060c5   43852d88
    std::string str = "sYmb0ls";
    std::string nonce = "9fb03375";
    std::vector<int16_t> str_bytes = string_to_bytes(str);
    std::vector<int16_t> nonce_bytes = hex_to_bytes(nonce);; 
     
     
    Hash32Desc *hash_desc = new Hash32Desc(str_bytes, nonce_bytes);
    hash_desc->generate_hash();
    //hash_desc->print_data();
    //hash_desc->make_padding();
    //hash_desc->print_data();
    //hash_desc->print_nonce();
    hash_desc->print_hash();

    std::string hash_str = bytes_to_hex(hash_desc->get_hash());
    for (size_t i = 0; i < hash_str.size(); i++)
    {
        std::cout << hash_str[i];
    }
    
    delete hash_desc;
}

void test6()
{
    //45a060c5   43852d88
    std::string str = "sYmb0ls";
    std::string hash = "43852d88";
    
    std::vector<int16_t> str_bytes = string_to_bytes(str);
    std::vector<int16_t> hash_bytes = hex_to_bytes(hash);; 
     
     
    NonceFinder *nonce_finder = new NonceFinder(str_bytes, hash_bytes);
    nonce_finder->prepare_data();
     
    delete nonce_finder;
}

int main()
{
    test6();
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