#pragma once
#ifndef TASK_EXECUTOR_HPP
#define TASK_EXECUTOR_HPP

#include <string>

class TaskExecutor final
{
public:
    static bool run(std::string const& commands);
};

#endif