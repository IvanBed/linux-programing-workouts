#include "linear_system_solver.hpp"

// Gaussian elimination

bool try_make_row_echelon_form(std::vector<std::vector<double>> &matrix, size_t start_pos)
{
    double el = matrix[start_pos][start_pos];
    double diff_coeff;
    for (size_t i = start_pos + 1; i < matrix.size(); i++)
    {
        if (is_zero_limit(matrix[i][start_pos]) == 0)
            continue;
		
        diff_coeff = matrix[start_pos][start_pos] / matrix[i][start_pos];
        
        if (is_zero_limit(diff_coeff) == 0)
        {
            swap_rows(matrix, start_pos, i);
        }
        else 
        {
            fill_row(matrix, i, (-1.0) * diff_coeff);
            add_rows(matrix, start_pos, i);            
        }
    }
    return true;
}

double find_xi(std::vector<std::vector<double>> const &matrix, int row, std::vector<double> res)
{
    double right_side_val = matrix[row][matrix[row].size() - 1];
    double xi             = 0;
    
    for (int i = row + 1; i < matrix[row].size() - 1; i++)
    {
        right_side_val -= res[i] * matrix[row][i];
    }
    xi = right_side_val/matrix[row][row];
    return xi;
}

std::vector<double> find_roots(std::vector<std::vector<double>> const &matrix)
{
    size_t x_cnt  = matrix[0].size() - 1;
	std::vector<double> res(x_cnt);
    double xi;
    
    for (int i = x_cnt - 1; i >= 0; i--)
    {
        xi = find_xi(matrix, i, res);  
        res[i] = xi;
    }
    return res;
}

void print_roots(std::vector<double> const &res)
{
    if (res.empty())
		return;
	
	for (size_t i =  0; i < res.size(); i++)
    {
        if (i == res.size() - 1)
            std::cout << std::setprecision(PRECISION) << res[i];
        else 
            std::cout << std::setprecision(PRECISION) << res[i] << " ";
    }  
}

uint8_t gaussian_method(std::vector<std::vector<double>> &matrix, std::vector<double> &solution)
{
	if (matrix.empty())
	{
		return NOT_CONSIST;
	}
	
	std::vector<double> res;
	for (size_t i = 0; i < matrix.size() - 1; i++)
    {
        try_make_row_echelon_form(matrix, i);
    }
	
	size_t vars_cnt = matrix[0].size() - 1;
	
	size_t base_matrix_rank = find_rank(matrix, matrix.size(), vars_cnt);
	size_t augmented_matrix_rank = find_rank(matrix, matrix.size(), vars_cnt + 1);

	if (base_matrix_rank < augmented_matrix_rank)
	{
		return NOT_CONSIST;
	}
	else 
	{
	    if (vars_cnt > base_matrix_rank)
		{
            return CONSIST_AND_INF_SOLUTION;
		}	
		else 
		{
			solution = find_roots(matrix);
		    return CONSIST_AND_ONE_SOLUTION;
		}	       
	}
}

void solve_linear_system()
{
	size_t rows;
	size_t columns;
	std::vector<double> solution;
	uint8_t res;
	
	std::cin >> rows;
	std::cin >> columns;
	std::vector<std::vector<double>> matrix(rows, std::vector<double>(columns + 1, 0));
	
	read_matrix(matrix);
	res = gaussian_method(matrix, solution);
	
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
		std::cout << "YES\n";
		print_roots(solution);
	}
}

