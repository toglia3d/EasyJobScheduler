#pragma once
#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include "Task.h"

using task_map = std::unordered_map<std::string, Task>;

class TaskManager final
{
public:
    /**
     * \brief Given the argument count and arguments char array supplied in the entry point
     *  of the application this function will check if they're enough to define at least one task name
     *  and its file path.
     * \param main_arguments_count main's entry point argument count
     * \param main_arguments main's entry point arguments
     * \param main_task task to be solved first
     * \param file_paths all file paths found will be placed here.
     * \return will be true if minimum amount of arguments are found.
     */
    static bool get_task_paths(
        int main_arguments_count,
        char** main_arguments,
        std::string& main_task,
        std::vector<std::string>& file_paths);

    /**
     * \brief Given a vector of file paths this function will check if all files exist and
     * 
     * \param paths 
     * \param tasks 
     * \param error 
     * \return 
     */
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

    static bool parse_task(
        std::string const& path,
        Task& task,
        std::string& error);

    static void validate_dependencies(
        Task const& current_task,
        task_map const& tasks,
        std::unordered_map<std::string, int>& visited,
        int& current_depth,
        bool& circular_dependency,
        std::string& heap_trace);

    static bool has_circular_dependency(
        Task const& task,
        task_map const& tasks,
        std::string& error);

    static void run_dependencies(
        Task const& current_task,
        task_map const& tasks,
        std::vector<std::string>& execution_order,
        bool& run_failed,
        std::string& error);

    static bool can_run_task(
        task_map const& tasks, 
        std::string& error);
};

#endif