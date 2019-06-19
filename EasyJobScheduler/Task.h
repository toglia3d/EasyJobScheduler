#pragma once
#ifndef TASK_H
#define TASK_H

//Author: Alberto Toglia June 2019

#include <string>
#include <unordered_map>
#include <vector>

struct Task final
{
    std::string                 m_name;
    std::string                 m_commands;
    std::vector<std::string>    m_dependencies;

    Task(   std::string&& name, 
            std::string&& commands, 
            std::vector<std::string> dependencies);

    Task();
};

#endif







