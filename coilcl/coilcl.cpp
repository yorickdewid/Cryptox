#include "Lexer.h"

#include <string>

__declspec(dllexport) void Compile(std::string content)
{
	Lexer{ content };
}
