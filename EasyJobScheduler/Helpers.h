#pragma once
#ifndef HELPERS_H
#define HELPERS_H

#include <vector>
#include <locale>

class Helpers
{
public:
    static std::vector<std::string> split_string(
        std::string const& argument, 
        char separator);

    static std::vector<std::string> parse_file_lines(std::string const& file_path);
};

#endif

