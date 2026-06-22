#ifndef BASE16_HPP
#define BASE16_HPP
#include <cmath>
#include <cctype>
#include <string>

#define NO_OFFSET    127
#define FIRST_OFFSET 191

#define FIRSTBYTE    208

struct charset_offset
{
	int first;
	int second;
};

std::string quoted_printable(std::string const &, std::string const &);

#endif