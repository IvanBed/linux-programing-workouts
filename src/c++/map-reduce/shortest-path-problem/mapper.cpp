#include "shortest_path_utils.hpp"

void mapper()
{
    std::string line = "";
    struct vertex_info cur_info;
    while (getline(std::cin, line)) {
        std::cout << line << "\n";
        parse_line(line, cur_info);
        if (!cur_info.adj_vertex.empty())
            print_adj_vertex_info(cur_info.adj_vertex, cur_info.dist);
        line = "";
    }
}

int main() 
{
    mapper();	
    return 0;
}