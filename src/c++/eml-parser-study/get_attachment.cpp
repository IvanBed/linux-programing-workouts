#include <iostream>
#include <fstream>
#include <sstream> 
#include <string>
#include <regex>
#include <cctype>
#include <iomanip>
#include "base64.hpp"

#define BOUNDARY_PATTERN " boundary=\"*"
#define ATTACH_PATTERN   "Content-Disposition: attachment*"
#define FILENAME_PATTERN "filename*"

std::string url_decode(std::string const &str) 
{
    std::string res;
    char        ch;
    int64_t     temp;

    for (size_t i = 0; i < str.length(); i++) {
        if (str[i]=='%') 
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

std::string get_boundary_val(std::string const & line)
{
    size_t start_pos = line.find('=');
    size_t offset    = line.size() - (start_pos + 3);

    std::string res =  line.substr(start_pos + 2, offset);
    return res;
}

std::string get_filename_val(std::string const & line)
{
    size_t start_pos   = line.find('=');
    size_t offset    = line.size() - (start_pos + 1);
    
    if (line[line.size() - 1] == ';')
    {
        offset--;
    }
        
    std::string res =  line.substr(start_pos + 1, offset);
    return res;
}

bool empty_line(std::string const & line)
{
    if (line.size() == 0 || line.empty() || line == "\n" ||  line == "\r\n" || line == "\r\n\r\n" || line == "" || line == " ")
        return true;
    else 
        return false;
}

bool save_str_to_file(std::string const & out_file_name, std::string const & data)
{
    std::ofstream out;
    out.open(out_file_name, std::ios::binary);
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

std::string get_attach_file_name(std::vector<std::string> const & vect)
{
    std::string res = "";

    for (auto & el : vect)
        res += el;

    return res;
}

int main(int argc, char *argv[])
{
    std::string file_name = "111.eml";
    if (argc == 2)
    {
        file_name = std::string(argv[1]);
    }

    std::ifstream eml_file(file_name);

    std::string line;

    std::regex bound_pattern(BOUNDARY_PATTERN);
    std::regex attach_pattern(ATTACH_PATTERN);
    std::regex filename_pattern(FILENAME_PATTERN);

    std::string attach_file_name        = "";

    bool attach_found            = false;
    bool attach_body_start_found = false;

    std::string attachment_data  = "";
    std::string cur_boundary     = "";
    std::string  decoded_attachment_data;

    std::vector<std::string> file_name_tokens;

    while (getline(eml_file, line))
    {
        if (regex_search(line, filename_pattern))
        {
            if (!attach_body_start_found)
            {
                file_name_tokens.push_back(url_decode(get_filename_val(line)));
            }
        }
        
        if (regex_search(line, bound_pattern))
        {
             std::string temp_boundary =  "--" + get_boundary_val(line);
             if (cur_boundary != temp_boundary)
                 cur_boundary = temp_boundary;
        }

        if (regex_search(line, attach_pattern))
        {
            attach_found = true;
            std::cout << "attachment found " << line  << std::endl;
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

                //std::cout << decoded << std::endl;  
                attach_file_name = get_attach_file_name(file_name_tokens);
                save_str_to_file(attach_file_name, decoded_attachment_data);
                attachment_data = "";
                
                std::cout << get_attach_file_name(file_name_tokens) << std::endl;
                file_name_tokens.clear();
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
