#include <bits/stdc++.h>
#include <iostream>
#include <vector>
#include <cstdint>
#include <iomanip>

#define CONSIST_AND_ONE_SOLUTION 0
#define CONSIST_AND_INF_SOLUTION 1
#define NOT_CONSIST              2

#define EPSILON 0.0000000000001
#define PRECISION 6


// Gaussian elimination

using namespace std;

void print_matrix(vector<vector<double>> &);

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

void add_rows(vector<vector<double>> &matrix, size_t row_a, size_t row_b)
{
    for (size_t i = 0; i < matrix[row_a].size(); i++)
    {
        matrix[row_b][i] = is_zero_limit(matrix[row_b][i] + matrix[row_a][i]);    
    }
}

void fill_row(vector<vector<double>> &matrix, size_t row, double diff_coeff)
{
    for (size_t i = 0; i < matrix[row].size(); i++)
    {
        matrix[row][i] = matrix[row][i] * diff_coeff;
    }
}

void swap_rows(vector<vector<double>> &matrix, size_t row_a, size_t row_b)
{ 
	vector<double> temp = matrix[row_b];
	matrix[row_b]       = matrix[row_a];
	matrix[row_a]       = temp;
}

size_t find_rank(vector<vector<double>> &matrix, size_t n, size_t m)
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

bool try_make_row_echelon_form(vector<vector<double>> &matrix, size_t start_pos)
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

double find_xi(vector<vector<double>> &matrix, int row, vector<double> res)
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

vector<double> find_roots(vector<vector<double>> &matrix)
{
    size_t x_cnt  = matrix[0].size() - 1;
	vector<double> res(x_cnt);
    double xi;
    
    for (int i = x_cnt - 1; i >= 0; i--)
    {
        xi = find_xi(matrix, i, res);  
        res[i] = xi;
    }
    return res;
}

void print_matrix(vector<vector<double>> &matrix)
{
    for (size_t i = 0; i < matrix.size(); i++)
    {
        for (size_t j = 0; j < matrix[i].size(); j++)
        {
            cout << matrix[i][j] << " ";
        }
        cout << endl;
    }
    cout << "------------------------\n";
}

void print_roots(vector<double> res)
{
    if (res.empty())
		return;
	
	for (size_t i =  0; i < res.size(); i++)
    {
        if (i == res.size() - 1)
            cout << setprecision(PRECISION) << res[i];
        else 
            cout << setprecision(PRECISION) << res[i] << " ";
    }  
}

uint8_t solve_linear_system(vector<vector<double>> &matrix, vector<double> &solution)
{
	if (matrix.empty())
	{
		return NOT_CONSIST;
	}
	
	vector<double> res;
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

void read_linear_system(vector<vector<double>> &matrix)
{
    for (auto & row : matrix)
	{
		for (auto & el : row)
		{
			cin >> el;
		}
	}
}

int main() 
{
	size_t rows;
	size_t columns;
	vector<double> solution;
	uint8_t res;
	
	cin >> rows;
	cin >> columns;
	vector<vector<double>> matrix(rows, vector<double>(columns + 1, 0));
	
	read_linear_system(matrix);
	res = solve_linear_system(matrix, solution);
	
	if (res== NOT_CONSIST)
	{
		cout << "NO\n";
	}
	else if (res == CONSIST_AND_INF_SOLUTION)
	{
		cout << "INF\n";	
	}
	else 
	{
		cout << "YES\n";
		print_roots(solution);
	}
}
