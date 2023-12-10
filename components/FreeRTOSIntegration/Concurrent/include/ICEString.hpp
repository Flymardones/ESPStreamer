#pragma once

#include <string>
#include <vector>
#include "ICEAllocator.hpp"

[[maybe_unused]] static std::vector<std::string> ICETokenize(const std::string& input, const std::string& delimiter)
{
	size_t pos_start = 0, pos_end, delim_len = delimiter.length();
	std::string token;
	std::vector<std::string> result;
	while ((pos_end = input.find(delimiter, pos_start)) != std::string::npos)
	{
		token = input.substr(pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		result.push_back(token);
	}
 
	result.push_back(input.substr(pos_start));
	return result;
}

typedef std::string ICEstring;

class ICEString : public std::string
{
    
    ICEString(ICEAllocator<char> alloc) : std::string(alloc)
    {

    }
};