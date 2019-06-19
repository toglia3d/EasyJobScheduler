#include "TaskExecutor.h"
#include <cstdlib>

bool TaskExecutor::run(std::string const& commands)
{
    return system(commands.c_str()) == 0;
}
