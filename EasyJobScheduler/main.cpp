#include <string>
#include <iostream>

#include "Task.h"
#include "Helpers.h"
#include "TaskManager.h"

int main(int argc, char *argv[])
{
    std::vector<std::string> paths;
    std::string task_name;
    
    if (!TaskManager::get_task_paths(argc, argv, task_name, paths))
    {
        std::cout << "Not valid arguments" << std::endl;
        return 0;
    }

    task_map tasks;
    std::string error;

    if (!TaskManager::parse_tasks(paths, tasks, error))
    {
        std::cout << error << std::endl;
        return 0;
    }

    if (!TaskManager::run(task_name, tasks, error))
    {
        std::cout << error << std::endl;
    }

    getchar();

    return 0;
}

