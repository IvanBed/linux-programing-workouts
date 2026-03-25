////#include <iostream>
#include <fstream>
#include <sstream> 
#include <string>
#include <regex>
#include <iostream>

#define BOUNDARY_PATTERN " boundary=\"*"
#define ATTACH_PATTERN   "Content-Disposition: attachment*"

std::string get_boundary_val(std::string const & line)
{
    size_t pos = line.find('=');

    std::string res =  line.substr(pos + 2, line.size() - (pos + 3));
    return res;
}

bool empty_line(std::string const & line)
{
    if (line.size() == 0 || line.empty() || line == "\n" ||  line == "\r\n" || line == "\r\n\r\n" || line == "" || line == " ")
    {
        return true;
    }
    else 
        return false;
}

bool save_str_to_file

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

    bool attach_found           = false;
    bool attach_body_startfound = false;

    std::string buff         = "";
    std::string cur_boundary = "";

    while (getline(eml_file, line))
    {
        if (regex_search(line, bound_pattern))
        {
             std::string temp_boundary =  "--" + get_boundary_val(line);
             if (cur_boundary != temp_boundary)
                 cur_boundary = temp_boundary;
        }

        if (attach_found && attach_body_startfound)
        {
            if (line == cur_boundary || empty_line(line))
            {
                attach_found           = false;
                attach_body_startfound = false;
                std::cout << buff << std::endl;
                buff = "";
            }
            else
            {
                buff += line;
            }
        }

        if (regex_search(line, attach_pattern))
        {
            attach_found = true;
            std::cout << "attachment found " << line  << std::endl;
        }

        if (attach_found && empty_line(line))
        {
            attach_body_startfound = true;
        }

    }
    eml_file.close();
}
