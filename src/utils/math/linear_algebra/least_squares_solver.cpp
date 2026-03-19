#include "linear_system_solver.hpp"

std::vector<std::vector<double>> least_squares(std::vector<std::vector<double>> const & matrix)
{
	std::vector<std::vector<double>> transpose_matrix = make_transpose_matrix(matrix);
    size_t var_cnt = get_vars_cnt(matrix);
	
	std::vector<std::vector<double>>coeff_matrix(var_cnt, std::vector<double>(var_cnt + 1, 0));
	
	for (size_t i = 0; i < coeff_matrix.size(); i++)
	{
		for (size_t j = 0; j < coeff_matrix[i].size(); j++)
		{
			 coeff_matrix[i][j] = dot_product(transpose_matrix[i], transpose_matrix[j]);
		}		
	}
	return coeff_matrix;
}

void solve()
{
    size_t rows;
	size_t columns;
	std::vector<double> solution;
	uint8_t res;
		
	std::cin >> rows;
	std::cin >> columns;
	std::vector<std::vector<double>> matrix(rows, std::vector<double>(columns + 1, 0));
	read_matrix(matrix);

	std::vector<std::vector<double>> coeff_matrix = least_squares(matrix);
	
	res = gaussian_method(coeff_matrix, solution);
	
	if (res== NOT_CONSIST)
	{
		std::cout << "NO\n";
	}
	else if (res == CONSIST_AND_INF_SOLUTION)
	{
		std::cout << "INF\n";	
	}
	else 
	{
		print_roots(solution);
	}
}
