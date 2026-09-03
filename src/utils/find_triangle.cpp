/*
Задача: количество треугольников.

Дан неориентированный граф без петель и кратных ребер. Найдите в нем количество треугольников. Треугольник - цикл длины 3.

Входные данные

В первой строке числа n и m (1 <= n, m <= 3000) - количество вершин и ребер в графе. Следующие m строк содержат ребра ai bi - числа от 1 до n.

Выходные данные

Одно число - количество треугольников.

Шаблоны

В этой задаче есть шаблоны кода для языков Python 3.10 и C++20.

Пояснение к примеру

В данном графе есть треугольники (1, 2, 3) и (1, 3, 4).
*/

#include <vector>
#include <iostream>
#include <queue>

struct VertexColors
{
    bool is_red;
    bool is_blue;
    bool is_black;
};

void print_graph(std::vector<std::vector<int>> &g)
{
    for (size_t i = 1; i < g.size(); i++)
    {
        std::cout << i << " : ";
        for (size_t j = 0; j < g[i].size(); j++)
        {
            std::cout << g[i][j] << " ";
        }
        std::cout << "\n";
    }
}

size_t trianglesCnt(std::vector<std::vector<int>> &g, std::vector<VertexColors> vc, std::vector<bool> &visited, int vertex)
{
    std::queue<int> q;
    
    vc[vertex].is_red = true;
    visited[vertex] = true;
	
	size_t cnt = 0;
	
    for (size_t i = 0; i < g[vertex].size(); i++)
    {
        if (!visited[g[vertex][i]])
        {
            q.push(g[vertex][i]);
            vc[g[vertex][i]].is_blue = true;
        }      
    }
    
    while (!q.empty())
    {
        int cur_vertex = q.front();
        q.pop();
        //std::cout << "current vertex: " << cur_vertex << "\n";
        vc[cur_vertex].is_red = true;
        for (size_t i = 0; i < g[cur_vertex].size(); i++)
        {
            if (!vc[g[cur_vertex][i]].is_red && !visited[g[cur_vertex][i]])
            {
                vc[g[cur_vertex][i]].is_black = true;
                if (vc[g[cur_vertex][i]].is_black && vc[g[cur_vertex][i]].is_blue)
                {
                    cnt++;
                }
            }
        }
    }
	return cnt;
}

int main() 
{
    std::ios_base::sync_with_stdio(0), std::cin.tie(0);

    int n, m;
    std::cin >> n >> m;
    std::vector<std::vector<int>> g(n + 1, std::vector<int>());
    std::vector<VertexColors> colors(n + 1, {false, false, false});
    std::vector<bool> visited(n + 1, false);	
	
    for (int i = 0; i < m; i++) 
    {
    int a, b;
    std::cin >> a >> b; // числа от 1 до n, концы двустороннего ребра
        g[a].push_back(b);
        g[b].push_back(a);
    }
    size_t cnt = 0;
    for (size_t i = 1; i < visited.size(); i++)
    {
        if (!visited[i])
            cnt += trianglesCnt(g, colors, visited, i);
    }
	std::cout << cnt << std::endl;
}
