#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include "Helpers.h"

class Task
{
public:
    std::string                 m_name;
    std::string                 m_commands;
    std::vector<std::string>    m_dependencies;
};






