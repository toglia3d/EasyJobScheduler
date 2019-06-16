#pragma once
#include "Task.h"

using task_map = std::unordered_map<std::string, Task>;

class TaskManager
{
public:
    static bool get_task_paths(
        int argc,
        char** argv,
        std::string& main_task,
        std::vector<std::string>& arguments);

    static bool parse_tasks(
        std::vector<std::string> const& paths,
        task_map& tasks,
        std::string& error);

    static bool run(
        std::string const& starting_name, 
        task_map const& tasks, 
        std::string& error);

private:
    static bool run(
        Task const& task,
        task_map const& tasks,
        std::string& error);

    static Task parse_task(
        std::string const& path,
        std::string& error);

    static void validate_dependencies(
        Task const& current_task,
        task_map const& tasks,
        std::unordered_map<std::string, int>& visited,
        int& current_depth,
        bool& circular_dependency,
        std::string& trace_str);

    static bool has_circular_dependency(
        Task const& task,
        task_map const& tasks,
        std::string& error);

    static void run_dependencies(
        Task const& current_task,
        task_map const& tasks,
        std::unordered_map<std::string, bool>& executed_tasks);

    static bool can_run_task(
        task_map const& tasks, 
        std::string& error);
};
