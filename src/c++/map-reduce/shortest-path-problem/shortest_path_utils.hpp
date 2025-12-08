#ifndef SHORTEST_PATH_UTILS
    
    #define SHORTEST_PATH_UTILS
    #include <vector>
    #include <iostream>
    
    struct vertex_info
    {
        size_t vertex_id;
        int64_t dist;
        std::vector<std::string> adj_vertex;
    };
    
    std::vector<std::string> split_line(std::string const &, char);
    size_t get_vertex_id(std::string const &);
    int64_t get_distance_value(std::string const &);
    std::vector<std::string> const get_adj_list(std::string const &);
    void parse_line(std::string const &, struct vertex_info &)
    
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
    
    
#endif