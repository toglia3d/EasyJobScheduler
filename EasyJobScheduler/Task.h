#pragma once
#ifndef TASK_H
#define TASK_H

#include <string>
#include <unordered_map>
#include <vector>

// Task Container 

class Task final
{
public:
    std::string                 m_name;
    std::string                 m_commands;
    std::vector<std::string>    m_dependencies;

    Task(   std::string&& name, 
            std::string&& commands, 
            std::vector<std::string> dependencies);

    Task();
};

#endif







