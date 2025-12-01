#include <iostream>
#include <vector>

std::vector<size_t> split_line(std::string const & line, char del)
{
    std::vector<size_t> res;
    
    std::string val = "";
    
    for (size_t i = 0; i < line.size(); i++)
    {
        if(line[i] != del)
        {
            val += line[i];
        }
        else 
        {
            res.push_back(std::stoi(val));
            val = "";
        }
    }        
    res.push_back(std::stoi(val));
    return res;
}

void parse_pair(std::string const & line, size_t & f, size_t & s)
{
    size_t empty_pos = line.find(" ");
    f = stoi(line.substr(0, empty_pos));
    s = stoi(line.substr(empty_pos + 1, line.size() - (empty_pos + 1)));
}

void read_input_data(std::vector<std::vector<size_t>> & graph, size_t & verts_cnt, size_t & edges_cnt, size_t & start_vert, size_t & target_vert)
{
    std::string line = "";
    getline(std::cin, line);
    
    parse_pair(line, verts_cnt, edges_cnt);

    size_t index = 0;
    while(getline(std::cin, line))
    {
        if (index < edges_cnt)
        {
            graph.push_back(split_line(line, ' '));
        }
        else 
        {
            parse_pair(line, start_vert, target_vert);
        }    
        index++;
    }
}

int print_graph(std::vector<std::vector<size_t>> const & graph)
{
    for (auto const & arr : graph)
    {
        for (auto const & el : arr)
            std::cout << el << " ";
        std::cout << "\n"; 
    }
    return 0;
}

int dijkstra(std::vector<std::vector<size_t>> const & graph, size_t & start_vert, size_t & target_vert)
{
    
    return -1;
}

int main() 
{
    std::vector<std::vector<size_t>> graph;
    size_t verts_cnt = 0;
    size_t edges_cnt = 0;
    
    size_t start_vert = 0;
    size_t target_vert = 0;
    
    read_input_data(graph, verts_cnt, edges_cnt, start_vert, target_vert);  
    print_graph(graph);
        
}