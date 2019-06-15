#pragma once

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class Task
{
public:
    std::string                 m_name;
    std::string                 m_commands;
    std::vector<std::string>    m_dependencies;
    Task(std::string && name, std::string && commands, std::vector<std::string> dependencies) :
    m_name(std::move(name)),
    m_commands(std::move(commands)),
    m_dependencies(std::move(dependencies)){}

    Task() :m_name(""),
        m_commands(""),
        m_dependencies() {}

};






