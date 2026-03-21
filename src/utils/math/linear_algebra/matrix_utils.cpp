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

void add_rows(std::vector<std::vector<double>> &matrix, size_t row_a, size_t row_b)
{
    for (size_t i = 0; i < matrix[row_a].size(); i++)
    {
        matrix[row_b][i] = is_zero_limit(matrix[row_b][i] + matrix[row_a][i]);    
    }
}

void fill_row(std::vector<std::vector<double>> &matrix, size_t row, double diff_coeff)
{
    for (size_t i = 0; i < matrix[row].size(); i++)
    {
        matrix[row][i] = matrix[row][i] * diff_coeff;
    }
}

void swap_rows(std::vector<std::vector<double>> &matrix, size_t row_a, size_t row_b)
{ 
	std::vector<double> temp = matrix[row_b];
	matrix[row_b]       = matrix[row_a];
	matrix[row_a]       = temp;
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

int64_t try_find_nonzero_el(std::vector<std::vector<double>> &matrix, size_t col)
{
    for (size_t i = col; i < matrix.size(); i++)
    {
        std::cout << matrix[i][col] << "\n";
        if (is_zero_limit(matrix[i][col]) != 0)
        {
            return i;
        }
    }
    return -1;
}

double get_determinant(std::vector<std::vector<double>> matrix, size_t pos)
{
    if (pos == matrix.size() - 1)
    {
        return matrix[pos][pos];
    }
    
    int64_t nonzero_el_pos;
    double determinant_sigh = 1;
    double sub_determinant  = 1;
    std::cout << "test\n";
    if (is_zero_limit(matrix[pos][pos]) == 0)
    {
        std::cout << "pos: " << pos << "\n";
        nonzero_el_pos = try_find_nonzero_el(matrix, pos);
        if (nonzero_el_pos == NOT_FOUND)
        {
            return 0;
        }
        else
        {
            determinant_sigh *= -1;
            swap_rows(matrix, pos, (size_t)nonzero_el_pos);
        }
    }
       
    double diff_coeff;
    sub_determinant = matrix[pos][pos];
    
    for (size_t i = pos + 1; i < matrix.size(); i++)
    {
        if (is_zero_limit(matrix[i][pos]) == 0)
            continue;
        
        diff_coeff = matrix[i][pos] / matrix[pos][pos];
        fill_row(matrix, pos, (-1.0) * diff_coeff);
        add_rows(matrix, pos, i);    
    }
    return determinant_sigh * sub_determinant * get_determinant(matrix, pos + 1);
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