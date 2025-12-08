#include <iostream>
#include <vector>

struct vertex_info
{
    size_t vertex_id;
    int64_t dist;
    std::vector<std::string> adj_vertex;
};

std::vector<std::string> split_line(std::string const & line, char separator)
{
    std::vector<std::string> res;
    std::string val = "";
    for (size_t i = 0; i < line.size(); i++)
    {
        if(line[i] != separator) {
            val += line[i];
        }
        else {
            if (val != "") res.push_back(val);
            val = "";
        }
    }
    if (val != "") res.push_back(val);

    return res;
}

size_t get_vertex_id(std::string const & line)
{
    size_t res;
    try {
        size_t tab_pos = line.find('\t');
        res = (size_t)std::stoi(line.substr(0, tab_pos));
    }
    catch (std::invalid_argument const & e) {
        std::cout << e.what() << "\n";
    }
    catch (std::out_of_range const & e) {
        std::cout << e.what() << "\n";
    }
    return res;
}

int64_t get_distance_id(std::string const & line)
{
    int64_t res;
    try {
        size_t start = line.find('\t') + 1;
        size_t end = line.find('\t', start);
        std::string dist = line.substr(start, end - start);
        if (dist == "INF")
            res = -1;
        else 
            res = (int64_t)std::stoi(dist);
    }
    catch (std::invalid_argument const & e) {
        std::cout << e.what() << "\n";
    }
    catch (std::out_of_range const & e) {
        std::cout << e.what() << "\n";
    }
    return res;
}

std::vector<std::string> const get_adj_list(std::string const & line)
{
    std::vector<std::string> res;
    try {
        size_t start = line.find('{') + 1;
        size_t end = line.find('}');
        std::string list_line = line.substr(start, end - start);
        res = split_line(list_line, ',');
    }
    catch (std::invalid_argument const & e) {
        std::cout << e.what() << "\n";
    }
    catch (std::out_of_range const & e) {
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

inline void print_vector(std::vector<std::string> const & vect, int64_t dist)
{
    std::string dist_str = "";
    if (dist == -1)
        dist_str = "INF";
    else 
        dist_str = std::to_string(++dist);
    
    for(auto const & el : vect)
        std::cout << el << "\t" << dist_str << "\t" << "{}\n";
}

void mapper()
{
    std::string line = "";
    struct vertex_info cur_info;
    while (getline(std::cin, line)) {
        std::cout << line << "\n";
        parse_line(line, cur_info);
        if (!cur_info.adj_vertex.empty())
            print_vector(cur_info.adj_vertex, cur_info.dist);
        line = "";
    }
}

int main() 
{
    mapper();	
    return 0;
}