#include <iostream>
#include <fstream>
#include <sstream> 
#include <string>
#include <regex>
#include <cctype>
#include <iomanip>
#include "base64.hpp"

#define BOUNDARY_PATTERN " boundary=\"*"
#define FILENAME_PATTERN "filename*"
// Header keys
#define ATTACH_PATTERN   "Content-Disposition: attachment*"
#define ENCODING_PATTERN "Content-Transfer-Encoding:*"
#define ID_PATTERN "Content-ID:*"
#define TYPE_PATTERN "Content-Type:*"
#define DESCRIPTION_PATTERN "Content-Description:*"

#define RFC2047HINT "=\?.*\?="
#define RFC2231HINT ".*'.*'.*"

#define RFC2047     "=\?([^?]+)\?([QBqb])\?([^?]+)\?="
#define RFC2231     ""

#define UNKOWN_ENCODE  0
#define RFC2047_ENCODE 1
#define RFC2231_ENCODE 2

void to_utf8(std::string &str)
{

}

static std::vector<std::string> split_line(std::string const & line, char separator)
{
    std::vector<std::string> res;
    std::string val = "";
    for (size_t i = 0; i < line.size(); i++) 
    {
        if(line[i] != separator) 
        {
            val += line[i];
        }
        else 
        {
            if (val != "") 
                res.push_back(val);
            
            val = "";
        }
    }
    if (val != "") res.push_back(val);

    return res;
}

static std::vector<std::string> encoded_words(std::string const & line, char separator)
{
    std::vector<std::string> res(3, "");
    
	size_t first_del_pos = line.find(separator);
    
	if (first_del_pos == std::string::npos)
	{
	    res[2] = line;
	    return res;
	}
	
	size_t second_del_pos = line.find(separator, first_del_pos + 1);
	
	res[0] = line.substr(0, first_del_pos);
	res[1] = line.substr(first_del_pos + 1, second_del_pos - (first_del_pos + 1));
	res[2] = line.substr(second_del_pos + 1);
	
    return res;
}

static std::string get_decoded_text_rfc2047(std::string & token)
{
	// Удаление обрамляющих =? для удобства разбиения
	std::string prepared_token = token.substr(2, token.size() - 4);
	
	std::string decoded_text = "";
    std::vector<std::string> sub_tokens = encoded_words(prepared_token, '?');
    
    std::string charset      = sub_tokens[0];
    std::string encoding     = sub_tokens[1];
    std::string encodedtext  = sub_tokens[2];
   
    if (charset == "utf-8")
    {
        to_utf8(encodedtext);
    }

    if (encoding == "B")
    {
        try
        {
            decoded_text = base64::from_base64(encodedtext);             
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            decoded_text = "";
        }
    }
	// Реализовать энкодинг Quoted-Printable
    else if (encoding == "Q")
    {
        decoded_text = encodedtext;
    }
	
    return decoded_text;
}

static std::string parse_rfc2047(std::string & str)
{
    std::string filename = "";

    std::regex rfc2027(RFC2047);

    std::vector<std::string> tokens = split_line(str, ' ');
    for (auto & token : tokens)
    {
        if (regex_search(token, rfc2027))
        {
            filename += get_decoded_text_rfc2047(token);
        }
    }
    return filename;
}

static std::string url_decode(std::string const &str) 
{
    std::string res;
    char        ch;
    int64_t     temp;

    for (size_t i = 0; i < str.length(); i++) {
        if (str[i] == '%') 
        {
            sscanf(str.substr(i + 1, 2).c_str(), "%x", &temp);
            ch   = static_cast<char>(temp);
            res += ch;
            i = i + 2;
        } 
        else 
        {
            res += str[i];
        }
    }
    return res;
}

static std::string get_decoded_text_rfc2231(std::string & token)
{
	std::string decoded_text = "";
    std::vector<std::string> sub_tokens = encoded_words(token, '\'');

    std::string charset     = sub_tokens[0];
    std::string lang        = sub_tokens[1];
    std::string encodedtext = sub_tokens[2];
    if (charset == "utf-8")
    {
        to_utf8(encodedtext);
    }
    
	decoded_text = url_decode(encodedtext);

    return decoded_text;
}

static std::string parse_rfc2231(std::string & str)
{
    std::string filename = "";

	filename = get_decoded_text_rfc2231(str);
    return filename;
}

static uint8_t check_encoding(std::string const & str)
{
    std::regex hint2047(RFC2047HINT);
    std::regex hint2231(RFC2231HINT);
    if (regex_search(str, hint2047))
    {
        return RFC2047_ENCODE;
    }
	if (regex_search(str, hint2231))
    {
        return RFC2231_ENCODE;
    }
	
    return UNKOWN_ENCODE;
}

//Изменить семантику функции, плохая практика изменения инпута
static void del_quotes_if_needed(std::string & str)
{
    if (str[0] == '"')
    {
        str.erase(0, 1);
    }
    if (str[str.size() - 1] == '"')
    {
        str.erase(str.size() - 1);
    }    
}

static std::string get_boundary_val(std::string const & line)
{
    size_t start_pos = line.find('=');
    size_t offset    = line.size() - (start_pos + 3);

    std::string res  =  line.substr(start_pos + 2, offset);
    return res;
}

static std::string get_filename_val(std::string const & line)
{
    size_t start_pos = line.find('=');
    size_t offset    = line.size() - (start_pos + 1);
    
    if (line[line.size() - 1] == ';')
    {
        offset--;
    }
        
    std::string res = line.substr(start_pos + 1, offset);
    del_quotes_if_needed(res);
    return res;
}

static bool empty_line(std::string const & line)
{
    if (line.empty() || line == "\n" || line == "\r" || line == "\r\n" || line == "\r\n\r\n" || line == "" || line == " ")
        return true;
    else 
        return false;
}

static bool save_str_to_file(std::string const & out_filename, std::string const & data)
{
    std::ofstream out;
    out.open(out_filename, std::ios::binary);
    if (!out.is_open()) {
        std::cerr << "Error: Unable to open file!" << std::endl;
        return 1;
    }
    else
    {
        out << data << std::endl;
    }
    out.close();

    return true;
}

static std::string get_attach_filename(std::vector<std::string> const & vect)
{
    std::string res = "";

    for (auto & el : vect)
        res += el;

    return res;
}

static void remove_carriage_return_if_needed(std::string & str)
{
    if (!str.empty() && str[str.size() - 1] == '\r')
        str.erase(str.size() - 1);
}

static bool header_key(std::string & line)
{
    std::regex id_pattern(ID_PATTERN);
    std::regex attach_pattern(ATTACH_PATTERN);
    std::regex encoding_pattern(ENCODING_PATTERN);
    std::regex description_pattern(DESCRIPTION_PATTERN);
    std::regex type_pattern(TYPE_PATTERN);
    std::regex filename_pattern(FILENAME_PATTERN);
    
    if (regex_search(line, id_pattern) || regex_search(line, attach_pattern) || regex_search(line, encoding_pattern) 
        || regex_search(line, description_pattern) || regex_search(line, type_pattern) || regex_search(line, filename_pattern))
    {
        return true;
    }
    else 
    {
        return false;
    }  
}

//Добавить парсинг кодировки Content-Transfer-Encoding для вложения
int main(int argc, char *argv[])
{
    std::string eml_filename = "";
    if (argc == 2)
    {
        eml_filename = std::string(argv[1]);
    }
	else
	{
		std::cout << ""
		return 1;
	}

    std::ifstream eml_file(eml_filename);

    std::string line;

    std::regex bound_pattern(BOUNDARY_PATTERN);
    std::regex attach_pattern(ATTACH_PATTERN);
    std::regex filename_pattern(FILENAME_PATTERN);

    std::string attach_filename = "";

    bool attach_found            = false;
    bool attach_body_start_found = false;
    bool filename_parsing_starts = false;

    std::string attachment_data  = "";
    std::string cur_boundary     = "";
    std::string decoded_attachment_data;
    
    std::string cur_filename  = "";
    std::vector<std::string> filename_tokens;

    while (getline(eml_file, line))
    {
        remove_carriage_return_if_needed(line);
        
		// Аккумуляция имени файла вложения на случай если значение этого имени располагается на нескольких строчках eml файла
		// Условия, если запущен процесс аккумуляции имени файла, текующая строка не пустая и не является токеном хедера или началом вложения
        if (!empty_line(line) && filename_parsing_starts && !header_key(line) && !attach_body_start_found)
        {
            cur_filename += line;
        }
        else if (filename_parsing_starts && (empty_line(line)  || header_key(line) || attach_body_start_found))
        {
            std::string filename = get_filename_val(cur_filename);
			
			filename_tokens.push_back(filename);
            filename_parsing_starts = false;
            cur_filename = "";
        }
        
		// Запуск аккумуляции имени файла
        if (regex_search(line, filename_pattern))
        {
            if (!filename_parsing_starts)
            {      
                cur_filename += line;
                //std::cout << cur_filename << " endline\n";
                filename_parsing_starts = true;    
            }
        }
		
		// Сохранение актуального значения границы
        if (regex_search(line, bound_pattern))
        {
             std::string temp_boundary =  "--" + get_boundary_val(line);
             if (cur_boundary != temp_boundary)
                 cur_boundary = temp_boundary;
        }

        // Флаг вложения
        if (regex_search(line, attach_pattern))
        {
            attach_found = true;
            //std::cout << "attachment found " << line  << std::endl;
        }

        if (attach_found && attach_body_start_found)
        {
            if (line == cur_boundary || empty_line(line))
            {
                attach_found            = false;
                attach_body_start_found = false;
                try
                {
                    decoded_attachment_data = base64::from_base64(attachment_data);  
                }
                catch(const std::exception& e)
                {
                    std::cerr << e.what() << '\n';
                    decoded_attachment_data = "";
                }

                //Из токенов файлов собираем название файла и декодируем его в зависимости от кодировки
				attach_filename = get_attach_filename(filename_tokens);
                
				switch(check_encoding(attach_filename))
				{
					case RFC2047_ENCODE: 
						attach_filename = parse_rfc2047(attach_filename);
						break;
					
					case RFC2231_ENCODE:
						attach_filename = parse_rfc2231(attach_filename);
						break;
					default:
						break;
				}

				save_str_to_file(attach_filename, decoded_attachment_data);
                attachment_data = "";

                std::cout << attach_filename << std::endl;
                filename_tokens.clear();
            }
            else
            {
                attachment_data += line;
            }
        }

        if (attach_found && empty_line(line))
        {
            attach_body_start_found = true;
        }
    }
    eml_file.close();
}
