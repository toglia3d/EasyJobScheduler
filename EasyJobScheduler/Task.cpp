#include "Task.h"
#include <utility>

Task::Task(std::string&& name, std::string&& commands, std::vector<std::string> dependencies) :
    m_name(std::move(name)),
    m_commands(std::move(commands)),
    m_dependencies(std::move(dependencies))
{}

Task::Task() : 
m_name(""),
m_commands(""),
m_dependencies({})
{}