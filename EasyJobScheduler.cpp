
#include <string>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <algorithm>

struct Task
{
    std::string m_name;
    std::string m_commands;
    std::vector<std::string> m_depenencies;
};

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
        word.erase(std::remove_if(word.begin(), word.end(), isspace), word.end());
        result.push_back(word);
        token_index = next_token_index;
    }
    return result;
}


Task readFile(std::string const& path)
{
    Task task;
    std::string line;
    std::ifstream myfile(path);
    if (myfile.is_open())
    {
        getline(myfile, task.m_name);
        std::string last_line;
        int lineCount = 1;
        while (getline(myfile, line))
        {
            last_line = line;
            task.m_commands += line;
            lineCount++;
        }

        if(lineCount > 2)
        {
            task.m_commands.erase(task.m_commands.size() - last_line.size(), last_line.size());
            task.m_depenencies = split(last_line, ',');
        }

        myfile.close();
    }
    else
    {
        std::cout << "Unable to open file";
    }

    return task;
}

bool checkTasks(std::vector<Task> const& tasks, std::string& error)
{
    for (const auto& taskA : tasks)
    {
        for (const auto& dependency : taskA.m_depenencies)
        {

            if (dependency == taskA.m_name)
            {
                std::cout << taskA.m_name << " has a circular Depency" << std::endl;
                return false;
            }

            bool found = false;
            for (auto const& taskB : tasks)
            {
                if (taskB.m_name == dependency)
                {
                    found = true;
                    break;
                }
            }

            if (!found)
            {
                std::cout << "Unable to find dependency " << dependency;
                return false;
            }
        }
    }

    return true;
}


void runDependencies(Task& currentTask, std::vector<Task>& tasks)
{
    for (const auto& dependency : currentTask.m_depenencies)
    {
        for (auto& task : tasks)
        {
            if (dependency == task.m_name)
            {
                if (task.m_depenencies.empty())
                {
                    std::cout << task.m_commands << std::endl;
                    break;
                }
                else
                {
                    runDependencies(task, tasks);
                }
            }
        }
    }

    std::cout << currentTask.m_commands << std::endl;
}

void run(std::string const& name, std::vector<Task>& tasks)
{

    for (auto& task : tasks)
    {
        if (task.m_name == name)
        {
            runDependencies(task, tasks);
        }
    }
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

    std::vector<Task> tasks;

    for (int i = 2; i< arguments.size(); ++i)
    {
        tasks.emplace_back(readFile(arguments[i]));
    }

    std::string error;
    if (checkTasks(tasks, error))
    {
        std::cout << error << std::endl;
    }

    run(arguments[1], tasks);

    getchar();
    
    return 0;
}

