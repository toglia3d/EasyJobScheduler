#pragma once
#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

//Author: Alberto Toglia June 2019

#include "Task.h"

using task_map = std::unordered_map<std::string, Task>;

class TaskManager final
{
public:
    /**
     * \brief Given the argument count and arguments char array supplied in the entry point
     *  of the application this function will check if they're enough to define at least one task name
     *  and its file path.
     */
    static bool get_task_paths(
        int main_arguments_count,
        char** main_arguments,
        std::string& main_task,
        std::vector<std::string>& file_paths,
        std::string& error);

    /**
     * \brief Given a vector of file paths this function will check if all files exist,
     *  if the names of the Tasks are unique, and attempt to parse them into a task_map.
     */
    static bool parse_tasks(
        std::vector<std::string> const& paths,
        task_map& tasks,
        std::string& error);

    /**
     * \brief Makes use of cstdlib::system to execute shell commands in Task::commands.
     * This will be done recursively starting by the Tasks with no dependencies and after the tree 
     * of execution was found to be free of circular dependencies.
     */
    static bool run(
        std::string const& starting_name, 
        task_map const& tasks, 
        std::string& error);

private:
    static bool run(
        Task const& task,
        task_map const& tasks,
        std::string& error);

    /**
     * \brief Checks if Task:
     * 1) Depends on itself.
     * 2) All dependencies were found.
     * 3) There is no circular dependency in tree
     */
    static bool can_run_task(
        task_map const& tasks,
        std::string& error);

    /**
     * \brief Given a file path this function will return true if it was able to parse the task name, 
     * the commands and the dependencies
     */
    static bool parse_task(
        std::string const& path,
        Task& task,
        std::string& error);

    /**
     * \brief Recursive function to traverse tree of dependencies to check whether tree has a circular dependency.
     */
    static void validate_dependencies(
        Task const& current_task,
        task_map const& tasks,
        std::unordered_map<std::string, int>& visited,
        int& current_depth,
        bool& circular_dependency,
        std::string& heap_trace);

    /**
     * \brief Check if Task dependencies have a circular dependency.
     */
    static bool has_circular_dependency(
        Task const& task,
        task_map const& tasks,
        std::string& error);

    /**
     * \brief Recursive function to execute commands with no dependencies first.
     */
    static void run_dependencies(
        Task const& current_task,
        task_map const& tasks,
        std::vector<std::string>& execution_order,
        bool& run_failed,
        std::string& error);
    
};

#endif