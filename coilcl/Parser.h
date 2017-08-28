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
	auto StorageClassSpecifier();
	auto TypeQualifier();
	std::unique_ptr<Value> TypeSpecifier();
	void FuncDef();
	void DeclarationSpecifier();
	void TranslationUnit();

private:
	Lexer lex;
	Token m_currentToken;
};

