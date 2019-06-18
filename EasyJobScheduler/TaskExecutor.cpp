#include "TaskExecutor.h"


bool TaskExecutor::run(std::string const& commands)
{
    return system(commands.c_str()) == 0;
}
