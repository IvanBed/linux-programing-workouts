#include "charset_decoder.hpp"

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
        UNKNOWN    = {0, 0};        
    } 
};

static std::string raw_bytes_win1251_to_utf8(std::vector<uint8_t> const & bytes, charset_offset const & offset)
{
    std::string utf8_str = "";
    if (bytes.empty())
    {
        std::cout << "WIN 1251 line is empty\n";
        return utf8_str;
    }

	uint8_t byte_1;
	uint8_t byte_2;	
	uint8_t byte_3;
	uint8_t byte_4;

	for (size_t i = 0; i < bytes.size(); i++)
	{
        if (bytes[i] < 127)
        {
            utf8_str += bytes[i];
        }
        else if (192 <= bytes[i] && bytes[i] <= 239)
        {
            utf8_str += FIRSTBYTE;
            utf8_str += (bytes[i] - offset.first);
        }
        else if (239 < bytes[i] && bytes[i] <= 255)
        {
            utf8_str += FIRSTBYTE + 1;
            utf8_str += (bytes[i] - (offset.first + offset.second));
        }
        else 
        {
            if (bytes[i] == 168)
            {
                utf8_str += FIRSTBYTE;
                utf8_str += (bytes[i] - offset.first);
            }
            else if (bytes[i] == 184) 
            {
                utf8_str += FIRSTBYTE + 1;
                utf8_str += (bytes[i] - (offset.first + offset.second));
            }
        }
	}
	
	return utf8_str;
}

static std::string raw_bytes_8859_5_to_utf8(std::vector<uint8_t> const & bytes, charset_offset const & offset)
{
    std::string utf8_str = "";
    if (bytes.empty())
    {
        std::cout << "8859 5 line is empty\n";
        return utf8_str;
    }

    uint8_t byte_1;
	uint8_t byte_2;	
	uint8_t byte_3;
	uint8_t byte_4;

	for (size_t i = 0; i < bytes.size(); i++)
	{
        if (bytes[i] < 176)
        {
            if (bytes[i] == 161)
            {
                utf8_str += FIRSTBYTE;
                utf8_str += (bytes[i] - offset.first);
            }
            else 
            {
                utf8_str += bytes[i];
            }
        }
        else if (176 <= bytes[i] && bytes[i] <= 223)
        {
            utf8_str += FIRSTBYTE;
            utf8_str += (bytes[i] - offset.first);
        }
        else if ((223 < bytes[i] && bytes[i] <= 239) || bytes[i] == 241)
        {
            utf8_str += FIRSTBYTE + 1;
            utf8_str += (bytes[i] - (offset.first + offset.second));
        }
	}
	
	return utf8_str;
}

static std::string  raw_bytes_koi_8r_to_utf8(std::vector<uint8_t> const & bytes)
{
    std::string utf8_str = "";
	if (bytes.empty())
    {
        std::cout << "KOI 8R line is empty\n";
        return utf8_str;
    }

    uint8_t byte_1;
	uint8_t byte_2;	
	uint8_t byte_3;
	uint8_t byte_4;

	for (size_t i = 0; i < bytes.size(); i++)
	{
        if (bytes[i] < OFFSET)
        {
            utf8_str += bytes[i];
        }
        else 
        {
            uint8_t index = bytes[i] + OFFSET;
            byte_1 = utf8[index][0];
            byte_2 = utf8[index][1];
            utf8_str += byte_1;
            utf8_str += byte_2;            
        }
	}

	return utf8_str;
}

static std::string vect_to_string(std::vector<uint8_t> const & bytes)
{
    std::string utf8_str = "";
    for (size_t i = 0; i < bytes.size(); i++)
    {
        utf8_str += bytes[i];
    }
    return utf8_str;
}

std::string raw_bytes_to_utf8(std::vector<uint8_t> const & bytes, std::string const & charset)
{
	std::string empty = "";
    if (bytes.empty())
    {
        std::cout << "Raw bytes line is empty\n";
        return empty;
    }

	if (charset.empty())
    {
        std::cout << "Charset is empty\n";
        return empty;
    }

    Charset charset_vals;
	if (charset == "ISO-8859-5")
		return raw_bytes_8859_5_to_utf8(bytes, charset_vals.ISO_8859_5);
    
	if (charset == "Windows-1251" || charset == "Win-1251" || charset == "win-1251" || charset == "windows-1251")
		return raw_bytes_win1251_to_utf8(bytes, charset_vals.Win_1251);	
	
	if (charset == "KOI-8R" || charset == "koi8-r")
		return raw_bytes_koi_8r_to_utf8(bytes);
	
    else 
        return vect_to_string(bytes);

	return "";
}
