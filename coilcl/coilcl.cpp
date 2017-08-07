#include "Parser.h"

#include <string>
#include <iostream>

__declspec(dllexport) void Compile(const std::string& content)
{
	Parser parsy{ content };
	parsy.Execute();
}
