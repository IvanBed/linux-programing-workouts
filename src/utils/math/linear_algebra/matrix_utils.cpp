#include "matrix_utils.hpp"

double is_zero_limit(double num)
{
    if (-1 * EPSILON < num && num < EPSILON)
        return 0;
    else
        return num;
}

double my_abs(double num)
{
    if (num < 0)
        return num * (-1);
    else
        return num;
}

double get_limit(double num)
{
	int64_t int_part = round(num);
	if (my_abs(num - int_part) < EPSILON)
		return (double)int_part;
	else 
		return num;
}

size_t find_rank(std::vector<std::vector<double>> const &matrix, size_t n, size_t m)
{
    size_t rank = 0;
    for (size_t i = 0; i < n; i++)
    {
        bool all_zeros = true;;
        for (size_t j = 0; j < m; j++)
        {
            if (is_zero_limit(matrix[i][j]) != 0)
                all_zeros = false;
        }
        if (!all_zeros) rank++;
    }
   return rank; 
}

double dot_product(std::vector<double> const &vect1, std::vector<double> const &vect2)
{
    if (vect1.empty() || vect2.empty())
    {
        return 0;
    }
     	 
    double dot_product = 0;
    
    for (size_t i = 0; i < vect1.size(); i++)
    {
        dot_product += get_limit(vect1[i] * vect2[i]);
		dot_product  = get_limit(dot_product);
    }
      
    return dot_product;
}

size_t get_vars_cnt(std::vector<std::vector<double>> const & matrix)
{
	return matrix[0].size() - 1;
}

std::vector<std::vector<double>> make_transpose_matrix(std::vector<std::vector<double>> const & matrix)
{
	std::vector<std::vector<double>> transpose_matrix(matrix[0].size(), std::vector<double>(matrix.size(), 0));
	
	for (size_t i = 0; i < transpose_matrix.size(); i++)
	{
		for (size_t j = 0; j < transpose_matrix[i].size(); j++)
		{
			transpose_matrix[i][j] = matrix[j][i];
		}		
	}
    return 	transpose_matrix;
}

void read_matrix(std::vector<std::vector<double>> &matrix)
{
    for (auto & row : matrix)
	{
		for (auto & el : row)
		{
			std::cin >> el;
		}
	}
}

void print_matrix(std::vector<std::vector<double>> const &matrix)
{
    for (size_t i = 0; i < matrix.size(); i++)
    {
        for (size_t j = 0; j < matrix[i].size(); j++)
        {
            std::cout << matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "------------------------\n";
}