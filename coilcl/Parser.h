#pragma once

#include "Lexer.h"

class Parser
{

public:
	Parser(const std::string& input);
	void Execute();

protected:
	void Error(const std::string& err);
	void NextToken();
	void ExpectToken(Token token);

private:
	int StorageClassSpecifier();
	void TranslationUnit();

private:
	Lexer lex;
	Token m_currentToken;
};

