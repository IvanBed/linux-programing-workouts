#include "shortest_path_utils.hpp"

void reducer()
{
    std::string line = "";
    
    struct vertex_info cur_info;
    struct vertex_info prev_info;
    
    getline(std::cin, line);
    parse_line(line, prev_info);
    
    std::vector<std::string> total_adj_vertexes;
    add_to_end(total_adj_vertexes, prev_info.adj_vertex);
    
    int64_t min_dist = prev_info.dist;
    
    while (getline(std::cin, line)) {
       parse_line(line, cur_info);
       if (cur_info.vertex_id != prev_info.vertex_id) {
           print_data(prev_info.vertex_id, min_dist, total_adj_vertexes);
           total_adj_vertexes.clear();
           add_to_end(total_adj_vertexes, cur_info.adj_vertex);
           min_dist = cur_info.dist;
       } else {
           merge_vects(total_adj_vertexes, cur_info.adj_vertex);
           if (cur_info.dist <= min_dist)
               min_dist = cur_info.dist;
       }
       prev_info = cur_info;
    }
    
    print_data(prev_info.vertex_id, min_dist, total_adj_vertexes);    
}

int main() 
{
    reducer();  
    return 0;
}
