#pragma once
#ifndef TASK_EXECUTOR_HPP
#define TASK_EXECUTOR_HPP

//Author: Alberto Toglia June 2019

#include <string>

class TaskExecutor final
{
public:
    /**
     * \brief Executes shell command using cstdlib::system
     */
    static bool run(std::string const& command);
};

#endif