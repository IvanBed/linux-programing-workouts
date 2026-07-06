#ifndef BASE16_HPP
#define BASE16_HPP
#include <cmath>
#include <cctype>
#include <string>

#include "charset_decoder.hpp"

#define FIRST_OFFSET 191


std::string quoted_printable(std::string const &, std::string const &);

#endif