#include "TaskManager.h"
#include "Helpers.h"
#include <vector>
#include <ostream>
#include <fstream>
#include <iostream>

bool TaskManager::get_task_paths(int argc, char** argv, std::string& main_task, std::vector<std::string>& arguments)
{
    if (argc < 3)
    {
        return false;
    }

    for (auto i = 2; i < argc; ++i)
    {
        arguments.emplace_back(argv[i]);
    }

    main_task = argv[1];
    return true;
}

bool TaskManager::parse_tasks(std::vector<std::string> const& paths, task_map& tasks, std::string& error)
{
    for (const auto& path : paths)
    {
        auto task = parse_task(path, error);

        if (error.empty())
        {
            tasks[task.m_name] = task;
        }
        else
        {
            return false;
        }
    }

    return true;
}

Task TaskManager::parse_task(std::string const& path, std::string& error)
{
    Task task;
    std::string line;
    std::ifstream my_file(path);
    if (my_file.is_open())
    {
        getline(my_file, task.m_name);
        std::string last_line;
        int lineCount = 1;
        while (getline(my_file, line))
        {
            last_line = line;
            task.m_commands += line;
            lineCount++;
        }

        if (lineCount > 2)
        {
            task.m_commands.erase(task.m_commands.size() - last_line.size(), last_line.size());
            task.m_dependencies = split(last_line, ',');
        }

        my_file.close();
    }
    else
    {
        error = "Unable to open file";
    }

    return task;
}

void TaskManager::validate_dependencies(Task const& current_task, task_map const& tasks,
    std::unordered_map<std::string, int>& visited, int& current_depth,
    bool& circular_dependency,
    std::string& trace_str)
{
    if (circular_dependency)
    {
        return;
    }

    trace_str += current_task.m_name + "->";
    visited[current_task.m_name] = current_depth;

    current_depth++;

    for (const auto& dependency_name : current_task.m_dependencies)
    {
        auto dependency_task_itr = tasks.find(dependency_name);
        if (dependency_task_itr != tasks.end())
        {
            auto& dependency_task = dependency_task_itr->second;
            auto visited_depth_itr = visited.find(dependency_task.m_name);
            if (visited_depth_itr != visited.end())
            {
                const auto visited_depth = visited_depth_itr->second;
                if (visited_depth < current_depth)
                {
                    circular_dependency = true;
                    trace_str += dependency_task.m_name;
                    break;
                }
            }

            validate_dependencies(dependency_task, tasks, visited, current_depth, circular_dependency, trace_str);
        }
    }

    current_depth--;
}

bool TaskManager::has_circular_dependency(Task const& task, task_map const& tasks, std::string& error)
{
    std::unordered_map<std::string, int> visited;
    bool circular_dependency_found = false;
    std::string trace_string;
    int depth = 0;

    validate_dependencies(task, tasks, visited, depth, circular_dependency_found, trace_string);

    if (circular_dependency_found)
    {
        error = trace_string;
    }

    return circular_dependency_found;
}

bool TaskManager::can_run_task(task_map const& tasks, std::string& error)
{
    for (const auto& task_itr : tasks)
    {
        auto& task = task_itr.second;

        for (const auto& dependency : task.m_dependencies)
        {
            if (dependency == task.m_name)
            {
                error = "Error: \"" + task.m_name + "\" depends on itself.";
                return false;
            }

            if (tasks.find(dependency) == tasks.end())
            {
                error = "Error: Unable to find dependency: \"" + dependency + "\"";
                return false;
            }
        }

        std::string dependency_tree;
        if (has_circular_dependency(task, tasks, dependency_tree))
        {
            error = "Error! Found circular dependency: \n" + dependency_tree;
            return false;
        }
    }

    return true;
}

inline void TaskManager::run_dependencies(Task const& current_task, task_map const& tasks,
    std::unordered_map<std::string, bool>& executed_tasks)
{
    for (const auto& dependency_name : current_task.m_dependencies)
    {
        auto dependency_task_itr = tasks.find(dependency_name);
        if (dependency_task_itr != tasks.end())
        {
            auto& dependency_task = dependency_task_itr->second;
            if (!dependency_task.m_dependencies.empty())
            {
                run_dependencies(dependency_task, tasks, executed_tasks);
            }

            if (executed_tasks.find(dependency_task.m_name) == executed_tasks.end())
            {
                std::cout << dependency_task.m_commands << std::endl;
                executed_tasks[dependency_task.m_name] = true;
            }
        }
    }
}

bool TaskManager::run(std::string const& starting_name, task_map const& tasks, std::string& error)
{
    auto& starting_task_itr = tasks.find(starting_name);
    if(starting_task_itr != tasks.end())
    {
        return TaskManager::run(starting_task_itr->second, tasks, error);
    }
    else
    {
        error = "Error! Task " + starting_name + " was not found.";
        return false;
    }
}
bool TaskManager::run(Task const& task, task_map const& tasks, std::string& error)
{
    if (can_run_task(tasks, error))
    {
        std::unordered_map<std::string, bool> executed_tasks;
        run_dependencies(task, tasks, executed_tasks);
        std::cout << task.m_commands << std::endl;
        return true;
    }

    return false;
}


