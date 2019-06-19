#pragma once
#ifndef HELPERS_H
#define HELPERS_H

//Author: Alberto Toglia June 2019

#include <vector>
#include <locale>

class Helpers
{
public:
    /**
     * \brief Separates string with defined separator into a vector of strings.
     */
    static std::vector<std::string> split_string(
        std::string const& argument, 
        char separator);

    /**
     * \brief Opens file and puts each line into a vector of strings.
     */
    static std::vector<std::string> parse_file_lines(
        std::string const& file_path);
};

#endif

