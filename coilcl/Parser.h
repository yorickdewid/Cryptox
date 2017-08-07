#pragma once

#include "Lexer.h"

class Parser
{

public:
	Parser(const std::string& input);
	void Execute();

private:
	Lexer lex;
};

