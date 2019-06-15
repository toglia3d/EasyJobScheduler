#pragma once

#include <cctype>
#include <vector>
#include <locale>
#include <algorithm>

static std::vector<std::string> split(std::string const& argument, const char separator)
{
    std::vector<std::string> result;

    std::string::size_type token_index = 0;
    while ((token_index = argument.find_first_not_of(separator, token_index)) != std::string::npos)
    {
        const auto next_token_index = argument.find_first_of(separator, token_index);
        auto word = argument.substr(token_index, next_token_index - token_index);
        word.erase(std::remove_if(word.begin(), word.end(), ::isspace), word.end());
        result.push_back(word);
        token_index = next_token_index;
    }
    return result;
}