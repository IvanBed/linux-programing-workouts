#include "base16.hpp"
#include <iostream>

struct Charset
{
	charset_offset ISO_8859_5;
	charset_offset Win_1251;
    charset_offset KOI_8R;
    charset_offset UNKNOWN;

    inline Charset()
    {
	    ISO_8859_5 = {32, 64};
	    Win_1251   = {48, 64};
        KOI_8R     = {82, 82};
        UNKNOWN    = {0, 0};        
    } 
};

static char get_hex_char_from_decimal_digit(int num)
{
    if (num > 15)
        return 'Z';
    
    switch(num)
    {    
        case 1:
            return '1';
        case 2:
            return '2';
        case 3:
            return '3';
        case 4:
            return '4';
        case 5:
            return '5';
        case 6:
            return '6';
        case 7:
            return '7';    
        case 8:
            return '8';        
        case 9:
            return '9';
        case 10:
            return 'A';
        case 11:
            return 'B';
        case 12:
            return 'C';
        case 13:
            return 'D';
        case 14:
            return 'E';
        case 15:
            return 'F';    
        default:
            return '0';        
    }
}

static bool is_hex_char(char hex_char)
{
    if ('A' <= hex_char && hex_char <= 'F' || '0' <= hex_char && hex_char <= '9')
        return true;
    else
        return false;
}

static int get_decimal_digit_from_hex_char(char hex_char)
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

static std::string decimal_to_hex(int64_t num)
{
    std::string hex_num = "";
    return hex_num;
}

static int64_t hex_to_decimal(std::string const & hex_num)
{
    int64_t res_num     = 0;
    size_t  start_pos   = hex_num.size() - 1;
    for(size_t power = 0; power < hex_num.size(); power++)
    {
       res_num += (int64_t) std::pow(16, power) * get_decimal_digit_from_hex_char(toupper(hex_num[start_pos - power]));
    }
    return res_num;
}

static void print_vector(std::vector<uint8_t>  v)
{
    for (size_t i = 0; i < v.size(); i++)
    {
        printf("%d ", v[i]);

    }
    std::cout << std::endl;
}

std::string quoted_printable(std::string const & str, std::string const & charset)
{
    std::string hex_token = "";
    std::string res       = "";	
	
    std::vector<uint8_t> hex_token_vector;
	
    for (size_t i = 0; i < str.size(); i++)
    {
        if (str[i] == '=')
        {
			hex_token = str.substr(i + 1, 2);

            hex_token_vector.push_back(hex_to_decimal(hex_token));
            
            print_vector(hex_token_vector);

			res += raw_bytes_to_utf8(hex_token_vector, charset);
            
            if (is_hex_char(hex_token[0]))
			    i += 2;
            else 
                i += 1;
            hex_token_vector.clear();
		}
		else 
		{
			res += str[i];
		}
    }	
	return res;
}
