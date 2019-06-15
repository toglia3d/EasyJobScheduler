#include <string>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cctype>


class Task
{
public:
    std::string m_name;
    std::string m_commands;
    std::vector<std::string> m_dependencies;
};

using task_map = std::unordered_map<std::string, Task>;

class Job
{
    std::unordered_map<std::string, std::string> m_tasks;
    std::unordered_map<std::string, std::vector<std::string>> m_dependencies;
};

static std::vector<std::string> split(std::string const& argument, const char separator)
{
    std::vector<std::string> result;

    std::string::size_type token_index = 0;
    while ((token_index = argument.find_first_not_of(separator, token_index)) != std::string::npos)
    {
        const auto next_token_index = argument.find_first_of(separator, token_index);
        auto word = argument.substr(token_index, next_token_index - token_index);
        word.erase(std::remove_if(word.begin(), word.end(), ::isspace), word.end());
        result.push_back(word);
        token_index = next_token_index;
    }
    return result;
}


Task read_file(std::string const& path, std::string& error)
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

        if(lineCount > 2)
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

void recurseCheckDependencies(
    Task const& current_task, 
    task_map const& tasks,
    std::unordered_map<std::string, int>& visited,
    int& current_depth,
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

            recurseCheckDependencies(dependency_task, tasks, visited, current_depth, circular_dependency, trace_str);
        }
    }

    current_depth--;
}

bool has_circular_dependency(
    Task const& task, 
    task_map const& tasks, 
    std::string& error)
{
    std::unordered_map<std::string, int> visited;
    bool circular_dependency_found = false;
    std::string trace_string;
    int depth = 0;

    recurseCheckDependencies(task, tasks, visited, depth, circular_dependency_found, trace_string);

    if (circular_dependency_found)
    {
        error = trace_string;
    }

    return circular_dependency_found;
}


bool can_run_task(task_map const& tasks, std::string& error)
{
    for (const auto& task_itr : tasks)
    {
        auto& task = task_itr.second;

        for (const auto& dependency : task.m_dependencies)
        {
            if (dependency == task.m_name)
            {
                error = "Error: \""+ task.m_name + "\" depends on itself.";
                return false;
            }

            if (tasks.find(dependency) == tasks.end())
            {
                error = "Error: Unable to find dependency: \"" + dependency+"\"";
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



void runDependencies(Task const& current_task, task_map const& tasks, std::unordered_map<std::string, bool>& executed_tasks)
{
    for (const auto& dependency_name : current_task.m_dependencies)
    {
        auto dependency_task_itr = tasks.find(dependency_name);
        if (dependency_task_itr != tasks.end())
        {
            auto& dependency_task = dependency_task_itr->second;
            if (!dependency_task.m_dependencies.empty())
            {
                runDependencies(dependency_task, tasks, executed_tasks);
            }

            if (executed_tasks.find(dependency_task.m_name) == executed_tasks.end())
            {
                std::cout << dependency_task.m_commands << std::endl;
                executed_tasks[dependency_task.m_name] = true;
            }
        }
    }
}

void run(Task const& task, task_map const& tasks)
{
    std::unordered_map<std::string, bool> executed_tasks;
    runDependencies(task, tasks, executed_tasks);
    std::cout << task.m_commands << std::endl;
}

int main(int argc, char *argv[])
{
    std::vector<std::string> arguments;
    arguments.reserve(argc);
    for (int i = 0; i < argc; ++i)
    {
        arguments.emplace_back(argv[i]);
    }

    if (arguments.size() < 3)
    {
        return 0;
    }

    task_map tasks;
    std::string error;

    for (int i = 2; i<arguments.size(); ++i)
    {
        auto task = read_file(arguments[i], error);

        if(error.empty())
        {
            tasks[task.m_name] = task;
        }
        else
        {
            std::cout << error << std::endl;
            break;
        }
    }

    if (can_run_task(tasks, error))
    {
        auto& main_task = tasks[arguments[1]];
        run(main_task, tasks);
    }
    else
    {
        std::cout << error << std::endl;
    }

    getchar();
    
    
    return 0;
}

