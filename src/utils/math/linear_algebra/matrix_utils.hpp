#ifndef MATRIX_UTILS
    #define MATRIX_UTILS

    #include <cmath>
    #include <vector>
    #include <cstdint>
    #include <iostream>

    #define EPSILON 0.0000000000001
    #define NOT_FOUND -1

    double is_zero_limit(double);
    double my_abs(double);
    double get_limit(double);
    void print_matrix(std::vector<std::vector<double>> const &);

    void read_matrix(std::vector<std::vector<double>> &);
    
    size_t find_rank(std::vector<std::vector<double>> const &, size_t, size_t);
    double dot_product(std::vector<double> const &, std::vector<double> const &);
    size_t get_vars_cnt(std::vector<std::vector<double>> const &);
    std::vector<std::vector<double>> make_transpose_matrix(std::vector<std::vector<double>> const &);
    
    void add_rows(std::vector<std::vector<double>> &, size_t, size_t);
    void fill_row(std::vector<std::vector<double>> &, size_t, double);
    void swap_rows(std::vector<std::vector<double>> &, size_t , size_t);
    double get_determinant(std::vector<std::vector<double>>, size_t);

#endif