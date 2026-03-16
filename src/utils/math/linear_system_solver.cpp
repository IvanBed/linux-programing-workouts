#include <iostream>
#include <vector>

using namespace std;

void print_matrix(vector<vector<double>> &);

double abs(double num)
{
    if (num < 0)
        return num * (-1);
    else
        return num;
}

void add_lines(vector<vector<double>> &matrix, size_t row_a, size_t row_b)
{
    for (size_t i = 0; i < matrix[row_a].size(); i++)
    {
        matrix[row_b][i] = matrix[row_b][i] + matrix[row_a][i];
    }
}

void fill_row(vector<vector<double>> &matrix, size_t row, double diff_coeff)
{
    cout << "diff_coeff: " << diff_coeff << endl;
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

bool try_make_row_echelon_form(vector<vector<double>> &matrix, size_t start_pos)
{
    double el = matrix[start_pos][start_pos];
    double diff_coeff;
    for (size_t i = start_pos + 1; i < matrix.size(); i++)
    {
        if (matrix[i][start_pos] == 0)
            continue;
		
        if (abs(el)< abs(matrix[i][start_pos])) 
        {
            swap_rows(matrix, start_pos, i);
        }
        diff_coeff = matrix[start_pos][start_pos] / matrix[i][start_pos];

        fill_row(matrix, i, (-1.0) * diff_coeff);
        add_lines(matrix, start_pos, i);
        
        print_matrix(matrix);
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
    vector<double> res(matrix.size());
    double xi;
    
    for (int i = matrix.size() - 1; i >= 0; i--)
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
    
    for (size_t i =  0; i < res.size(); i++)
    {
        cout << res[i] << " ";
    }  
}

bool solve_linear_system(vector<vector<double>> &matrix, vector<double> solution)
{
	vector<double> res;
	for (size_t i = 0; i < matrix7.size() - 1; i++)
    {
        try_make_row_echelon_form(matrix7, i);
    }
    solution = find_roots(matrix7);
	return true;
}

void test()
{
	vector<vector<double>> matrix = {{4,2,1}, {7,8,9}, {9, 1, 3}};
    vector<vector<double>> matrix1 = {{4,2,1,6}, {7,8,9,4}, {9, 1, 3, 5}, {4, 2, 3, 12}};

    vector<vector<double>> matrix2 = {{-1, -2, 2}, {-7, 1, -7}, {-1, 1, -2}};
    vector<vector<double>> matrix3 = {{4, 2, 1, 1}, {7, 8, 9, 1}, {9, 1, 3, 2}};
  
    vector<vector<double>> matrix4  = {{4,2,3,-11},   {0  -1, -2, 1},   {-6, 5, 3, 0}};
    
    vector<vector<double>> matrix5  = {{-2, -2, 7, 24} ,  { -4, -3, 4, 25},   {5, 1 ,2,-9}};
    
    vector<vector<double>> matrix6  = {{-8, 0, 1, -12},  {1, 1, 1, -2},  {-7, 0, 6, -31}};
    
    vector<vector<double>> matrix7  = {{-4, -8, 5, -6},  {-3, 1, -2, -14},  {0, -1, 1, 1}};
	vector<double> solution;
	solve_linear_system(matrix7, solution);
	print_roots(solution);
}

int main() 
{
    

    
 
    
}
