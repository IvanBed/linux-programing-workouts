#ifndef LINEAR_SYSTEM_SOLVER_CPP
    #define LINEAR_SYSTEM_SOLVER_CPP
    
    #include <vector>
    #include <iomanip>
    
    #include "matrix_utils.hpp"

    #define CONSIST_AND_ONE_SOLUTION 0
    #define CONSIST_AND_INF_SOLUTION 1
    #define NOT_CONSIST              2
    
    #define PRECISION 6
 
    uint8_t gaussian_method(std::vector<std::vector<double>> &, std::vector<double> &);
    void print_roots(std::vector<double> const &);
    
#endif