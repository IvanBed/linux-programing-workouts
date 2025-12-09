#include "shortest_path_utils.hpp"

static std::vector<std::string> split_line(std::string const & line, char separator)
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

static size_t get_vertex_id(std::string const & line)
{
    size_t res;
    try {
        size_t tab_pos = line.find('\t');
        res = (size_t)std::stoi(line.substr(0, tab_pos));
    } catch (std::invalid_argument const & e) {
        std::cout << e.what() << "\n";
    } catch (std::out_of_range const & e) {
        std::cout << e.what() << "\n";
    }
    return res;
}

static int64_t get_distance_value(std::string const & line)
{
    int64_t res;
    try {
        size_t start = line.find('\t') + 1;
        size_t end = line.find('\t', start);
        std::string dist = line.substr(start, end - start);
        if (dist == "INF")
            res = INT_MAX;
        else 
            res = (int64_t)std::stoi(dist);
    } catch (std::invalid_argument const & e) {
        std::cout << e.what() << "\n";
    } catch (std::out_of_range const & e) {
        std::cout << e.what() << "\n";
    }
    return res;
}

static std::vector<std::string> const get_adj_list(std::string const & line)
{
    std::vector<std::string> res;
    try {
        size_t start = line.find('{') + 1;
        size_t end = line.find('}');
        std::string list_line = line.substr(start, end - start);
        res = split_line(list_line, ',');
    } catch (std::invalid_argument const & e) {
        std::cout << e.what() << "\n";
    } catch (std::out_of_range const & e) {
        std::cout << e.what() << "\n";
    }
    return res;
}

void parse_line(std::string const & line, struct vertex_info & cur)
{
    cur.vertex_id = get_vertex_id(line);
    cur.dist = get_distance_value(line);
    cur.adj_vertex = get_adj_list(line);
}

static void print_vertex_id(size_t vertex_id)
{
    std::cout << vertex_id << "\t";
}

static void print_dist(int64_t dist)
{
    if (dist == INT_MAX)
        std::cout << "INF\t";
    else 
        std::cout << dist <<"\t";
}

static print_adj_vertex(std::vector<std::string> const & vect)
{
    if (vect.empty()) {
        std::cout << "{}\n";
        return;
    }

    std::cout << "{";
    for(size_t i = 0; i < vect.size(); i++) {
        if (i != vect.size() - 1)
            std::cout << vect[i] << ",";
        else 
            std::cout << vect[i] << "}\n";
    }      
}

void print_data(size_t vertex_id, int64_t dist, std::vector<std::string> const & vect)
{
    print_vertex_id(vertex_id);
    print_dist(dist);
    print_adj_vertex(vect);
    
}

void merge_vects(std::vector<std::string> & dest, std::vector<std::string> const & source)
{
    std::map<std::string, bool> mp;
    for (auto const & el: dest)
        mp[el] = true;

    for (auto & el: source) {
        if (auto search = mp.find(el); search == mp.end())
            dest.push_back(el);
    }
}

void add_to_end(std::vector<std::string> & dest, std::vector<std::string> const & source)
{
    for (auto & el: source)
        dest.push_back(el);
}