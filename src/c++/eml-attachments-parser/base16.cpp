#include "base16.hpp"
#include <iostream>

#define NO_OFFSET    127
#define FIRST_OFFSET 191

#define FIRSTBYTE    208


struct Charset
{
	charset_offset ISO_8859_5;
	charset_offset Win_1251;
    charset_offset KOI_8R;
    charset_offset UNKNOWN;

    Charset()
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

static std::string decode_hex(std::string const & hex_token, charset_offset const & offset)
{
	std::string res = "";
	int64_t byte_1;
	int64_t byte_2;	
    byte_1 = FIRSTBYTE;
	byte_2 = (int64_t) hex_to_decimal(hex_token);
	
	std::cout << "byte_2:" << hex_to_decimal(hex_token) << "\n";
	
	if (byte_2 > NO_OFFSET)
		byte_2-= offset.first;
	
	if (byte_2 > FIRST_OFFSET)
	{
		byte_1 += 1;
		byte_2 -= offset.second;
	}
	res += (char)byte_1;
	res += (char)byte_2;
	
	std::cout << hex_token << "\n";
	std::cout << res << "\n";
	return res;
}

static std::string decode_hex_with_charset(std::string const & hex_token, std::string const & charset, Charset const & charset_vals)
{
    if (charset == "ISO-8859-5")
		return decode_hex(hex_token, charset_vals.ISO_8859_5);
    
	if (charset == "Windows-1251" || charset == "Win-1251")
		return decode_hex(hex_token, charset_vals.Win_1251);	
	
	//if (charset == "KOI-8R")
	//	return decode_hex(hex_token, charset_vals.KOI_8R);
	
	return std::to_string(hex_to_decimal(hex_token));
}

std::string quoted_printable(std::string const & str, std::string const & charset)
{
    std::string hex_token = "";
    std::string res       = "";	
	
	Charset charset_vals;
	//std::cout << str << std::endl;
	//std::cout << charset << std::endl;
	
    for (size_t i = 0; i < str.size(); i++)
    {
        if (str[i] == '=')
        {
			hex_token = str.substr(i + 1, 2);
			res += decode_hex_with_charset(hex_token, charset, charset_vals);
			i += 2;
		}
		else 
		{
			res += str[i];
		}
    }	
	return res;
}
