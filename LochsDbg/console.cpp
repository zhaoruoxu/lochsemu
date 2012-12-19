#include "stdafx.h"
#include "console.h"


Console::Console()
{
}

Console::~Console()
{
    
}

std::string Console::ReadLine()
{
    std::string line;
    getline(std::cin, line, '\n');
    return line;
}
