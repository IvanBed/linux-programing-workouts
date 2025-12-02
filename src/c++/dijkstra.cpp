#include <iostream>
#include <vector>
#include <climits>
#include <cstring>

#define VERTEX(cnt) (cnt + 1) 

/*
    Наивная реализация алгоритма Дейкстры 
	Находит кратчайшие пути от одной из вершин графа до заданой другой вершины. 
	TO DO
    1. Оптимизация
    
*/

std::vector<int> split_line(std::string const & line, char separator)
{
    std::vector<int> res;
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
                res.push_back(std::stoi(val));
                val = "";
			}
        }
        res.push_back(std::stoi(val));
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

void parse_pair(std::string const & line, int & f, int & s)
{
    try
	{
	    size_t empty_pos = line.find(" ");
        f = stoi(line.substr(0, empty_pos));
        s = stoi(line.substr(empty_pos + 1, line.size() - (empty_pos + 1)));		
	}
	catch (std::invalid_argument const & e) 
	{
        std::cout << e.what() << "\n";
    }
    catch (std::out_of_range const & e) 
	{
        std::cout << e.what() << "\n";
    }
}

std::vector<std::vector<int>> read_input_data(int & verts_cnt, int & edges_cnt, int & start_vert, int & target_vert)
{
    std::string line = "";
    getline(std::cin, line);
    
    parse_pair(line, verts_cnt, edges_cnt);

    std::vector<std::vector<int>> graph(verts_cnt + 1, std::vector<int>(verts_cnt + 1));

    int index = 0;
    while(getline(std::cin, line))
    {
        if (index < edges_cnt)
        {
            std::vector<int> splited_line = split_line(line, ' ');
            graph[splited_line[0]][splited_line[1]] = splited_line[2];
        }
        else 
        {
            parse_pair(line, start_vert, target_vert);
        }    
        index++;
    }
    return graph;
}

int print_graph(std::vector<std::vector<int>> const & graph)
{
    for (size_t i = 1; i < graph.size(); i++)
    {
        for (size_t j = 1; j < graph[i].size(); j++)
            std::cout << graph[i][j] << " ";
        std::cout << "\n"; 
    }
    return 0;
}

inline void fill(int * arr, int size, int value)
{
    for (int i = 0; i < size; i++)
        arr[i] = value;
}

int dijkstra(std::vector<std::vector<int>> const & graph, int start_vert, int target_vert, int verts_cnt)
{
    int dist[verts_cnt + 1];
    int visited[verts_cnt + 1];
    std::vector<int> active_verts;
    
    fill(dist, verts_cnt + 1, INT_MAX);
    fill(visited, verts_cnt + 1, 0);

    dist[start_vert] = 0;
    visited[start_vert] = 1;
    active_verts.push_back(start_vert);
    
    int inter_cnt = 0;
     
    while(inter_cnt < (verts_cnt * verts_cnt) && visited[target_vert] == 0)
    {
        inter_cnt++;
        int min_dist = INT_MAX;
        int new_vertex = 1;
        int cur_vert;
        for (size_t j = 0; j < active_verts.size(); j++)
        {
            cur_vert = active_verts[j];
            for (size_t i = 1; i < graph[cur_vert].size(); i++)
            {
                if (graph[cur_vert][i] == 0)
                    continue;
                int cur_dist = graph[cur_vert][i] + dist[cur_vert];
                if (cur_dist < min_dist && !visited[i])
                {
                    min_dist = cur_dist;
                    new_vertex = i;
                }  
            }
        }
        active_verts.push_back(new_vertex);
        dist[new_vertex] = min_dist;
        visited[new_vertex] = 1;
    }
    
    return dist[target_vert] != INT_MAX? dist[target_vert] : -1;
}

int main() 
{
    int verts_cnt = 0;
    int edges_cnt = 0;
    
    int start_vert = 0;
    int target_vert = 0;
    
    std::vector<std::vector<int>> graph = read_input_data(verts_cnt, edges_cnt, start_vert, target_vert);  
    std::cout << dijkstra(graph, start_vert, target_vert, verts_cnt);    
}
