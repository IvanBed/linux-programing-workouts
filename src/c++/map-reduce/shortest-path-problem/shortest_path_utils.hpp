#ifndef SHORTEST_PATH_UTILS
    
    #define SHORTEST_PATH_UTILS
    #include <vector>
    #include <iostream>
    #include <climits>
    #include <map>
    
    struct vertex_info
    {
        size_t vertex_id;
        int64_t dist;
        std::vector<std::string> adj_vertex;
    };
    
    void parse_line(std::string const &, struct vertex_info &);
    
    void print_data(size_t, int64_t, std::vector<std::string> const &);

    void merge_vects(std::vector<std::string> &, std::vector<std::string> const &);

    void add_to_end(std::vector<std::string> &, std::vector<std::string> const &);
 
    inline void print_adj_vertex_info(std::vector<std::string> const & vect, int64_t dist)
    {
        std::string dist_str = "";
        if (dist == INT_MAX)
            dist_str = "INF";
        else 
            dist_str = std::to_string(++dist);
    
        for(auto const & el : vect)
            std::cout << el << "\t" << dist_str << "\t" << "{}\n";
    }
    
    
#endif
