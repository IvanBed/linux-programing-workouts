#include "get_attachments.hpp"
#include <fcntl.h>
#include <unistd.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
// TO DO 
// Добавить:
// Проверку charset
// Сбор информации по кодировкам
// Предкомпиляцию регулярных выражений Done

std::string quoted_printable(std::string const &);

struct RegexPatternsBase
{
    std::regex filename_pattern;
	std::regex anti_virus_pattern;
	
    explicit RegexPatternsBase() : filename_pattern(FILENAME_PATTERN, std::regex_constants::icase),
							   anti_virus_pattern(ANTIVIRUS_PATTERN, std::regex_constants::icase)
    {}

};

struct RegexPatterns
{
    std::regex id_pattern;
    std::regex encoding_pattern;
    std::regex description_pattern;
    std::regex type_pattern;
	
	std::regex attach_pattern;
    std::regex filename_pattern;
    std::regex filesize_pattern;
	
	std::regex filecd_pattern;
	std::regex filemd_pattern;
	std::regex filerd_pattern;
	
	std::regex anti_virus_pattern;
    std::regex hint2047;
    std::regex hint2231;
    
    explicit RegexPatterns() : id_pattern(ID_PATTERN), encoding_pattern(ENCODING_PATTERN), description_pattern(DESCRIPTION_PATTERN), 
							   type_pattern(TYPE_PATTERN), attach_pattern(ATTACH_PATTERN), filename_pattern(FILENAME_PATTERN, std::regex_constants::icase),
							   filesize_pattern(FILESIZE_PATTERN, std::regex_constants::icase), filecd_pattern(FILECD_PATTERN, std::regex_constants::icase),
							   filemd_pattern(FILEMD_PATTERN, std::regex_constants::icase), filerd_pattern(FILERD_PATTERN, std::regex_constants::icase),
							   anti_virus_pattern(ANTIVIRUS_PATTERN, std::regex_constants::icase), hint2047(RFC2047HINT), hint2231(RFC2231HINT)
    {}

};

struct HeaderPatterns
{
    std::string id_pattern;
    std::string encoding_pattern;
    std::string description_pattern;
    
    std::string type_pattern;
	std::string attach_pattern;
    std::string filesize_pattern;
	
	std::string filecd_pattern;
	std::string filemd_pattern;
	std::string filerd_pattern;

    RegexPatternsBase regex_patterns;

    explicit HeaderPatterns() : id_pattern(ID_PATTERN_F), encoding_pattern(ENCODING_PATTERN_F), description_pattern(DESCRIPTION_PATTERN_F), 
							   type_pattern(TYPE_PATTERN_F), attach_pattern(ATTACH_PATTERN_F), 
							   filesize_pattern(FILESIZE_PATTERN_F), filecd_pattern(FILECD_PATTERN_F),
							   filemd_pattern(FILEMD_PATTERN_F), filerd_pattern(FILERD_PATTERN_F)
    {}

    bool is_header_find(std::string const & line)
    {
        if (line.find(id_pattern) != std::string::npos || line.find(encoding_pattern) != std::string::npos 
           || line.find(description_pattern) != std::string::npos || line.find(type_pattern) != std::string::npos
           || line.find(attach_pattern) != std::string::npos || line.find(filesize_pattern) != std::string::npos
           || line.find(filecd_pattern) != std::string::npos || line.find(filemd_pattern) != std::string::npos || line.find(filerd_pattern) != std::string::npos) 
        {
            return true;
        }
        else 
        {
            return false;
        }  
    }

    bool is_header_regex(std::string const & line)
    {
        if (regex_search(line, regex_patterns.filename_pattern) || regex_search(line, regex_patterns.anti_virus_pattern)) 
        {
            return true;
        }
        else 
        {
            return false;
        }  
    }

    bool is_header(std::string const & line)
    {
        return is_header_find(line) || is_header_regex(line);
    }
};

/*struct Charset
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
};*/

static bool contain(std::vector<char> const & vect, char ch)
{
    auto result{std::find(begin(vect), std::end(vect), ch)};
    if (result != std::end(vect))
        return true;
    else
        return false;
}

static std::vector<std::string> split_line(std::string const & line, std::vector<char> const & separators)
{
    std::vector<std::string> res;
    std::string val = "";
    
	for (size_t i = 0; i < line.size(); i++) 
    {
        if(!contain(separators, line[i])) 
        {
			val += line[i];
			std::cout << "subtoken: " << val << " ";
        }
        else 
        {
            if (val != "") 
                res.push_back(std::move(val));
            
			std::cout << "token: " << val << " ";
            val = "";
        }
    }
    if (val != "") 
		res.push_back(std::move(val));

    return res;
}

static std::string check_unix_filename(std::string const & filename)
{
	std::string res = "";
	for (size_t i = 0; i < filename.size(); i++)
	{
		
		
	}
	return res;
}

static std::vector<std::string> encoded_words(std::string const & line, char separator)
{
    std::vector<std::string> res(3, "");
    std::string val = "";
    
    size_t first_sep_pos = line.find(separator);
    
    if (first_sep_pos == std::string::npos)
    {
        res[2] = line;
        return res;
    }
    
    size_t second_sep_pos = line.find(separator, first_sep_pos + 1);

    res[0] = line.substr(0, first_sep_pos);
    res[1] = line.substr(first_sep_pos + 1, second_sep_pos - (first_sep_pos + 1));
    res[2] = line.substr(second_sep_pos + 1);
    
    return res;
}

static std::vector<std::string> encoded_words_new(std::string const & line, char separator)
{   
    std::vector<std::string> res(3, "");
    std::string val = "";
    
    size_t start_token_pos = 0;
    size_t index = 0;
    while (index < line.size())
    {       
        size_t start_token_pos = line.find(separator, index);
        size_t end_token_pos   = line.find(separator, start_token_pos + 1);
        res.push_back(line.substr(start_token_pos + 1, end_token_pos - (start_token_pos + 1)));
        index = end_token_pos;
    }   
        
    return res;
}

std::vector<uint8_t> get_vect(std::string const & str)
{
    std::vector<uint8_t> res;
    for (size_t i = 0; i < str.size(); i++)
    {
        res.push_back((uint8_t)str[i]);
    }
    return res;
}

void print_vector(std::vector<uint8_t>  v)
{
    std::cout << "print_vector\n";
    for (size_t i = 0; i < v.size(); i++)
    {
        printf("%d ", v[i]);

    }
    std::cout << std::endl;
}

static std::string get_decoded_text_rfc2047(std::string & token)
{
    // Удаление обрамляющих =? для удобства разбиения
    std::string prepared_token = token.substr(2, token.size() - 4);
    
    std::string decoded_text = "";
    std::vector<uint8_t> decoded_bytes;

    std::vector<std::string> sub_tokens = encoded_words(prepared_token, '?');
    
    std::string charset      = sub_tokens[0];
    std::string encoding     = sub_tokens[1];
    std::string encodedtext  = sub_tokens[2];
    
    std::cout << "---------------------------" << std::endl;
    std::cout << charset << std::endl;
    std::cout << encoding << std::endl;
	std::cout << encodedtext << std::endl;
    std::cout << "---------------------------" << std::endl;

    if (encoding == "B" || encoding == "b")
    {
        try
        {
            decoded_bytes = base64_decode_to_bytes(encodedtext);         
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            //decoded_text = "";
        }

        //std::vector<uint8_t> dt = get_vect(decoded_text);
        print_vector(decoded_bytes);
		decoded_text = raw_bytes_to_utf8(decoded_bytes, charset);
			
    }
    else if (encoding == "Q" || encoding == "q")
    {
        decoded_text = quoted_printable(encodedtext, charset);
    }
	
    std::cout << "decoded_text: " << decoded_text << std::endl;
    return decoded_text;
}

static std::string parse_rfc2047(std::string const & str)
{
    std::string filename = "";

    std::regex rfc2027(RFC2047);
    std::vector<char> separators{' ', '\n', '\r', '\t'};
    std::vector<std::string> tokens = split_line(str, separators);
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
            sscanf(str.substr(i + 1, 2).c_str(), "%lx", &temp);
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

static std::string get_decoded_text_rfc2231(std::string const & token)
{
    std::string decoded_text = "";
    std::vector<std::string> sub_tokens = encoded_words(token, '\'');

    std::string charset     = sub_tokens[0];
    std::string lang        = sub_tokens[1];
    std::string encodedtext = sub_tokens[2];
    
	std::cout << charset << std::endl;
    std::cout << lang << std::endl;
	std::cout << encodedtext << std::endl;   
	
    decoded_text = url_decode(encodedtext);

    return decoded_text;
}

static std::string parse_rfc2231(std::string & str)
{
    std::string filename = "";

    filename = get_decoded_text_rfc2231(str);
    return filename;
}

static encode_result check_encoding(std::string const & str)
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

static bool save_str_to_file_linux(std::string const & out_filename, std::string const & data)
{
	ssize_t write_cnt;
    int fd;

    fd = open(out_filename.c_str(), O_WRONLY | O_CREAT | O_LARGEFILE, 0644);
	if (fd < 0)
	{
		return false;
	}
    write_cnt = write(fd, data.c_str(), data.size());
	close(fd);
	if (write_cnt != data.size())
        return false;
	else
	    return true;
}

static bool save_str_to_file(std::string const & out_filename, std::string const & data)
{
    std::ofstream out;
	std::cout << "Filename: " << out_filename << std::endl;
	
    out.open(out_filename, std::ios::binary);
    if (!out.is_open()) 
    {
        if (save_str_to_file_linux(out_filename, data))
		{
			return true;
		}
		else 
		{
		    std::cerr << "Error: Unable to write to file! Error number: " << std::strerror(errno) << std::endl;
			return false;
		}   
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

static bool header_key(std::string const & line, RegexPatterns const & patterns)
{   
    if (regex_search(line, patterns.id_pattern) || regex_search(line, patterns.attach_pattern) || regex_search(line, patterns.encoding_pattern) 
        || regex_search(line, patterns.description_pattern) || regex_search(line, patterns.type_pattern) || regex_search(line, patterns.filename_pattern) 
	        ||  regex_search(line, patterns.filesize_pattern) ||  regex_search(line, patterns.filecd_pattern)
			 ||  regex_search(line, patterns.filemd_pattern) ||  regex_search(line, patterns.filerd_pattern) || regex_search(line, patterns.anti_virus_pattern)) 
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
        std::cerr << "Enter the path to the file" << std::endl;
        return 1;
    }
	
    std::ifstream eml_file(eml_filename);

    if (!eml_file.is_open()) 
    {
        std::cerr << "Error: Unable to open EML file! Check the file and try again!\n Error number: " << std::strerror(errno) << std::endl;
        return 1;
    }
    
    std::cout << "eml file name " << eml_filename << "\n";

    build_decoding_table();

    std::string line;

    std::regex bound_pattern(BOUNDARY_PATTERN);
    std::regex attach_pattern(ATTACH_PATTERN);
    std::regex filename_pattern(FILENAME_PATTERN);

    RegexPatterns  patterns;
    HeaderPatterns header_patterns;

    std::string attach_filename = "";

    bool attach_found            = false;
    bool attach_body_start_found = false;
    bool filename_parsing_starts = false;

    std::string attachment_data  = "";
    std::string cur_boundary     = "";
    std::string decoded_attachment_data;
    
    std::string cur_filename  = "";
    std::vector<std::string> filename_tokens;
   
    size_t attachments_cnt = 0;
    while (getline(eml_file, line))
    {
        remove_carriage_return_if_needed(line);
        
        // Аккумуляция имени файла вложения на случай если значение этого имени располагается на нескольких строчках eml файла
        // Условия, если запущен процесс аккумуляции имени файла, текующая строка не пустая и не является токеном хедера или началом вложения
        if (!empty_line(line) && filename_parsing_starts && !header_patterns.is_header(line) /*!header_key(line, patterns)*/ && !attach_body_start_found)
        {
			cur_filename += line;
        }
        else if (filename_parsing_starts && (empty_line(line) || header_patterns.is_header(line) /*header_key(line, patterns)  */|| attach_body_start_found))
        {
            std::string filename = get_filename_val(cur_filename);
            //std::cout << "filename: "<< filename << "\n";
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
           // std::cout << "attachment found " << line  << std::endl;
        }

        if (attach_found && attach_body_start_found)
        {
            
		    if (line.find(cur_boundary) != std::string::npos || empty_line(line))
            {
                //std::cout << "cur_boundary: "<< cur_boundary << "\n";
				attach_found            = false;
                attach_body_start_found = false;
				attachments_cnt++;
				//std::cout << "attachment DATA: "<< attachment_data << "\n";
                try
                {
                    decoded_attachment_data = base64_decode_to_string(attachment_data);  
                }
                catch(const std::exception& e)
                {
                    std::cerr << e.what() << '\n';
                    decoded_attachment_data = "";
                }

                //Из токенов файлов собираем название файла и декодируем его в зависимости от кодировки
                attach_filename = get_attach_filename(filename_tokens);
                std::cout << "attach_filename 111: " << attach_filename << std::endl;
                //std::cout << "decoded_attachment_data: " << decoded_attachment_data << std::endl;
				if (attach_filename.empty())
					attach_filename = "temp" + std::to_string(attachments_cnt);
				
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
                std::cout << "attach_filename after decode: " << attach_filename << std::endl;
                if (!save_str_to_file(attach_filename, decoded_attachment_data))
				{
					std::cout << "Could not save the attachment!\n";
					
				}                    
                else
				{
					std::cout << attach_filename << std::endl;
				}
                attachment_data = "";   
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
	std::cout << "Total attachments found: " << attachments_cnt << std::endl;
	
    base64_cleanup();
    eml_file.close();
}
