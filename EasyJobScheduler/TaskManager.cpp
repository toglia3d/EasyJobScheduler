#include "TaskManager.h"

#include <vector>
#include <fstream>
#include "Helpers.h"
#include "TaskExecutor.h"

bool TaskManager::get_task_paths(
    int main_arguments_count, 
    char** main_arguments, 
    std::string& main_task, 
    std::vector<std::string>& file_paths,
    std::string& error)
{
    if (main_arguments_count < 3)
    {
        error = "Error: Arguments are not valid.";
        return false;
    }

    for (auto i = 2; i < main_arguments_count; ++i)
    {
        file_paths.emplace_back(main_arguments[i]);
    }

    main_task = main_arguments[1];
    return true;
}

bool TaskManager::parse_tasks(
    std::vector<std::string> const& paths, 
    task_map& tasks, 
    std::string& error)
{
    for (const auto& path : paths)
    {
        Task task;

        if (tasks.find(task.m_name) != tasks.end())
        {
            error = "Error: Task name \"" + task.m_name + "\" was found in multiple files.";
            return false;
        }

        if (parse_task(path, task, error))
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

bool TaskManager::parse_task(std::string const& path, Task& task, std::string& error)
{
    try
    {
        std::ifstream my_file(path);
        if (my_file.is_open())
        {
            std::string last_line;
            int line_count = 1;

            getline(my_file, task.m_name);
            std::string line;
            while (getline(my_file, line))
            {
                last_line = line;
                task.m_commands += line;
                line_count++;
            }

            if (line_count < 2)
            {
                error = "Error: Task "+ task.m_name +" is incomplete.";
                return false;
            }

            if (line_count > 2)
            {
                task.m_commands.erase(task.m_commands.size() - last_line.size(), last_line.size());
                task.m_dependencies = Helpers::split_string(last_line, ',');
            }

            my_file.close();
        }
        else
        {
            error = "Error: Unable to open file " + path + ".";
            return false;
        }
    }
    catch (...)
    {
        error = "Error: While reading file " + path + ".";
        return false;
    }

    return true;
}

void TaskManager::validate_dependencies(
    Task const& current_task, 
    task_map const& tasks,
    std::unordered_map<std::string, int>& visited, 
    int& current_depth,
    bool& circular_dependency,
    std::string& heap_trace)
{
    if (circular_dependency)
    {
        return;
    }

    heap_trace += current_task.m_name + "->";
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
                    heap_trace += dependency_task.m_name;
                    break;
                }
            }

            validate_dependencies(dependency_task, tasks, visited, current_depth, circular_dependency, heap_trace);
        }
    }

    current_depth--;
}

bool TaskManager::has_circular_dependency(
    Task const& task, 
    task_map const& tasks, 
    std::string& error)
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

bool TaskManager::can_run_task(
    task_map const& tasks, 
    std::string& error)
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
            error = "Error: Found circular dependency: \n" + dependency_tree;
            return false;
        }
    }

    return true;
}

inline void TaskManager::run_dependencies(
    Task const& current_task, 
    task_map const& tasks,
    std::vector<std::string>& execution_order,
    bool& run_failed,
    std::string& error)
{
    if (run_failed)
    {
        return;
    }

    for (const auto& dependency_name : current_task.m_dependencies)
    {
        auto dependency_task_itr = tasks.find(dependency_name);
        if (dependency_task_itr != tasks.end())
        {
            auto& dependency_task = dependency_task_itr->second;
            if (!dependency_task.m_dependencies.empty())
            {
                run_dependencies(dependency_task, tasks, execution_order, run_failed, error);
            }

            if (std::find( 
                execution_order.begin(), 
                execution_order.end(), 
                dependency_task.m_name) == execution_order.end())
            {
                if (!TaskExecutor::run(dependency_task.m_commands))
                {
                    run_failed = true;
                    error = "Error: Task \"" + dependency_task.m_name + "\" has not ended successfully.";
                    return;
                }

                execution_order.emplace_back(dependency_task.m_name);
            }
        }
    }
}

bool TaskManager::run(
    std::string const& starting_name, 
    task_map const& tasks, 
    std::string& error)
{
    const auto starting_task_itr = tasks.find(starting_name);
    if(starting_task_itr != tasks.end())
    {
        return run(starting_task_itr->second, tasks, error);
    }
    else
    {
        error = "Error: Task \"" + starting_name + "\" was not found.";
        return false;
    }
}

bool TaskManager::run(
    Task const& task, 
    task_map const& tasks, 
    std::string& error)
{
    if (!can_run_task(tasks, error))
    {
        return false;
    }

    std::vector<std::string> execution_order;
    bool failed = false;

    run_dependencies(task, tasks, execution_order, failed, error);

    if (failed)
    {
        return false;
    }

    if (!TaskExecutor::run(task.m_commands))
    {
        error = "Error: Task \"" + task.m_name + "\" has not ended successfully.";
        return false;
    }

    execution_order.emplace_back(task.m_name);

    return true;
}


