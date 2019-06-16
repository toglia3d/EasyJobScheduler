#pragma once

#include <vector>
#include <locale>

class Helpers
{
public:
    static std::vector<std::string> split_string(
        std::string const& argument, 
        char separator);
};



