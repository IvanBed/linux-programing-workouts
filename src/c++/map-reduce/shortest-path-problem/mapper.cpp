#include <iostream>
#include <vector>

struct vertex_info
{
    size_t vertex_id;
    size_t dist;
    std::vector<size_t> adj_vertex;
};

std::vector<size_t> split_line(std::string const & line, char separator)
{
    std::vector<size_t> res;
	try
	{
        std::string val = "";
        for (size_t i = 0; i < line.size(); i++)
        {
            if(line[i] != separator)
            {
                val += line[i];
            }
            else 
            {
                res.push_back((size_t)std::stoi(val));
                val = "";
			}
        }
        res.push_back((size_t)std::stoi(val));
    }  
    catch (std::invalid_argument const & e) 
	{
        std::cout << e.what() << "\n";
    }
    catch (std::out_of_range const & e) 
	{
        std::cout << e.what() << "\n";
    }
    return res;
}

size_t get_vertex_id(std::string const & line)
{
    size_t res;
    try
    {
        size_t tab_pos = line.find('\t');
        res = (size_t)stoi(line.substr(0, tab_pos));
    }
    catch (std::invalid_argument const & e) 
    {
        std::cout << e.what() << "\n";
    }
    catch (std::out_of_range const & e) 
    {
        std::cout << e.what() << "\n";
    }
    return res;
}

size_t get_distance_id(std::string const & line)
{
    size_t res;
    try
    {
        size_t start = line.find('\t') + 1;
        size_t end = line.find('\t', start);
        res = (size_t)stoi(line.substr(start, end - start));
    }
    catch (std::invalid_argument const & e) 
    {
        std::cout << e.what() << "\n";
    }
    catch (std::out_of_range const & e) 
    {
        std::cout << e.what() << "\n";
    }
    return res;
}

std::vector<size_t> get_adj_list(std::string const & line)
{
    std::vector<size_t> res;
    try
    {
        size_t start = line.find('{') + 1;
        size_t end = line.find('}');
        std::string list_line = line.substr(start, end - start);
        res = split_line(list_line, ',');
    }
    catch (std::invalid_argument const & e) 
    {
        std::cout << e.what() << "\n";
    }
    catch (std::out_of_range const & e) 
    {
        std::cout << e.what() << "\n";
    }
    return res;
}

void parse_line(std::string const & line, struct vertex_info & cur)
{
    cur.vertex_id = get_vertex_id(line);
    cur.dist = get_distance_id(line);
    cur.adj_vertex = get_adj_list(line);
}


int main() {
    
    std::string line = "";
    
    while (getline(std::cin, line))
    {
        
        
        
    }
    
    return 0;
}