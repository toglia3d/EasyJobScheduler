#include <string>
#include <iostream>

#include "Task.h"
#include "Helpers.h"
#include "TaskManager.h"

int main(int argc, char *argv[])
{
    std::vector<std::string> paths;
    std::string task_name;
    std::string error;

    if (!TaskManager::get_task_paths(argc, argv, task_name, paths, error))
    {
        std::cout << error << std::endl;
        return -1;
    }

    task_map tasks;
    if (!TaskManager::parse_tasks(paths, tasks, error))
    {
        std::cout << error << std::endl;
        return -1;
    }

    if (!TaskManager::run(task_name, tasks, error))
    {
        std::cout << error << std::endl;
        return -1;
    }

    return 0;
}

