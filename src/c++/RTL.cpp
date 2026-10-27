#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <fstream>

enum EncodingType
{
    SINGLE,
    SHORT,
    MEDIUM,
    LIST,
    LONG_LIST
};

struct LenInfo
{
    size_t data_len;
    size_t header_len;
};

struct EncodingTypeDesc
{
    EncodingType     type;
    int64_t          first_byte;
    std::string      first_byte_hex;
};

struct RLPEncodeState
{
    LenInfo          len_info;
    EncodingTypeDesc encoding_desc;
    std::string      hex_str;
};

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

EncodingTypeDesc get_encoding_desc(std::string const &hex_byte)
{
    int64_t byte = hex_to_decimal(hex_byte);
    EncodingTypeDesc encoding_desc;
    encoding_desc.first_byte = byte;
    encoding_desc.type = LONG_LIST;
    encoding_desc.first_byte_hex  = hex_byte;
    if (0x00 <= byte && byte <= 0x7f)
        encoding_desc.type = SINGLE;

    if (0x80 <= byte && byte <= 0xb7)
        encoding_desc.type = SHORT;

    if (0xb8 <= byte && byte <= 0xbf)
        encoding_desc.type = MEDIUM;

    if (0xc0 <= byte && byte <= 0xf7)
        encoding_desc.type = LIST;

    return encoding_desc;
}

LenInfo get_len_info(std::string const &hex_str, EncodingTypeDesc encoding_desc)
{
    size_t data_len   = 0;
    size_t len_of_len = 0;

    LenInfo len_info;

    switch(encoding_desc.type)
    {
        case SINGLE:
            data_len = 1;
            break;
        case SHORT:
            data_len = encoding_desc.first_byte - 0x80;
            break;
        case MEDIUM:
            len_of_len = encoding_desc.first_byte - 0xb7;
            if (len_of_len == 1)
                data_len = hex_to_decimal(hex_str.substr(0, 2));
            else
                data_len = hex_to_decimal(hex_str.substr(2, len_of_len));
            break;
        case LIST:
            data_len = encoding_desc.first_byte - 0xc0;
            break;
        case LONG_LIST:
            len_of_len = encoding_desc.first_byte - 0xf7 ;
            if (len_of_len == 1)
                data_len = hex_to_decimal(hex_str.substr(0, 2));
            else
                data_len = hex_to_decimal(hex_str.substr(2, len_of_len));
            break;
    }

    len_info.data_len = data_len;
    len_info.header_len = len_of_len + 1;

    return len_info;
}

size_t byte_to_hex_len(size_t len_in_bytes)
{
    return 2*len_in_bytes;
}

std::string get_head_key_token(RLPEncodeState &current_state)
{
    size_t offset = byte_to_hex_len(current_state.len_info.header_len);
    if (offset >  current_state.hex_str.size())
        return "";

    return current_state.hex_str.substr(0, byte_to_hex_len(current_state.len_info.header_len));
}

std::pair<std::string, std::string> get_key_value(RLPEncodeState &current_state)
{
    if (current_state.encoding_desc.type == SINGLE)
        return {"", current_state.hex_str.substr(0, byte_to_hex_len(current_state.len_info.data_len))};
    else
        return {current_state.hex_str.substr(0, byte_to_hex_len(current_state.len_info.header_len)), current_state.hex_str.substr(byte_to_hex_len(current_state.len_info.header_len), byte_to_hex_len(current_state.len_info.data_len))};
}

bool has_next_token(RLPEncodeState &current_state)
{
    if (byte_to_hex_len(current_state.len_info.data_len + current_state.len_info.header_len) < current_state.hex_str.size())
        return true;
    else
        return false;
}

std::string get_next_token(RLPEncodeState &current_state)
{
    size_t offset = byte_to_hex_len(current_state.len_info.data_len + current_state.len_info.header_len);
    if (offset > current_state.hex_str.size())
        return "";

    return current_state.hex_str.substr(byte_to_hex_len(current_state.len_info.data_len + current_state.len_info.header_len));
}

std::string get_firstbyte(std::string &hex_str)
{
    if (hex_str.size() < 2)
        return "";

    return hex_str.substr(0, 2);
}

void init_tokens(std::string &hex_str, std::vector<std::pair<std::string, std::string>> &tokens)
{
    if (hex_str.empty())
        return;

    RLPEncodeState                      current_state;
    std::pair<std::string, std::string> token_key_value;
    std::string                         next_token;

    current_state.encoding_desc  = get_encoding_desc(hex_str.substr(0, 2));
    current_state.len_info = get_len_info(hex_str, current_state.encoding_desc);
    current_state.hex_str = hex_str;

    if (current_state.encoding_desc.type == LIST || current_state.encoding_desc.type == LONG_LIST)
    {
        tokens.push_back({get_head_key_token(current_state), ""});
        hex_str.erase(0, byte_to_hex_len(current_state.len_info.header_len));
        init_tokens(hex_str, tokens);
    } 
    else
    {
        token_key_value = get_key_value(current_state);
        tokens.push_back(token_key_value);

        if (has_next_token(current_state))
        {
            next_token = get_next_token(current_state);
            init_tokens(next_token, tokens);
        }
    }
}

void hex_reverse(std::string &hexstr)
{
    size_t l = 1;
    size_t r = hexstr.size() - 1;
    char temp1;
    char temp2;

    while (l < r)
    {
        temp1 = hexstr[l - 1];
        temp2 = hexstr[l];

        hexstr[l - 1] = hexstr[r - 1];
        hexstr[l] = hexstr[r];

        hexstr[r - 1] = temp1;
        hexstr[r] = temp2;
        l += 2;
        r -= 2;
    }
}

void check_rlp(std::string &str)
{
    if (str.empty())
        return;

    std::vector<std::pair<std::string, std::string>> tokens;
    EncodingTypeDesc ed = get_encoding_desc(str.substr(0, 2));

    init_tokens(str, tokens);

    if (ed.type == LIST || ed.type == LONG_LIST)
    {
        reverse(tokens.begin() + 1, tokens.end());
    }
    else
    {
        hex_reverse(tokens[0].second);
    }
    for (auto const& x : tokens)
    {
        std::cout << x.first << x.second;
    }
    std::cout << "\n";
}

int main(int argc, char **argv)
{
    std::ifstream file(argv[1]);

    std::string line;

    if (file.is_open())
    {
        while (getline(file, line))
        {
            if (line.empty())
                continue;
            check_rlp(line);
        }

        file.close();
    }
    else
    {
        std::cerr << "Unable to open file!" << std::endl;
    }
    return 0;
}