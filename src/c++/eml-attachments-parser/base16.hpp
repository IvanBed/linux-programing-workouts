#ifndef BASE16_HPP
#define BASE16_HPP
#include <cmath>
#include <cctype>
#include <string>

struct charset_offset
{
	int first;
	int second;
};

std::string quoted_printable(std::string const &, std::string const &);

#endif