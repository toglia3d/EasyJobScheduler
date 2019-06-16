#include "TaskExecutor.h"


void TaskExecutor::run(std::string const& commands)
{
    system(commands.c_str());
}
