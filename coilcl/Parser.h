#pragma once

#include "Lexer.h"
#include "AST.h"

#include <stack>

class Parser
{

public:
	Parser(const std::string& input);
	void Execute();

protected:
	void Error(const std::string& err);
	void ExpectToken(Token token);
	void ExpectIdentifier();

	inline void Parser::NextToken()
	{
		m_lastToken = m_currentToken;
		m_currentToken = static_cast<Token>(lex.Lex());

		if (lex.HasData()) {
			m_lastData = m_currentData;
			m_currentData = lex.Data();
		}
	}

	inline bool Parser::TokenHasData() const
	{
		return lex.HasData();
	}

private:
	auto StorageClassSpecifier();
	auto TypeQualifier();
	std::unique_ptr<Value> TypeSpecifier();
	void Expression();
	void FuncDef();
	void JumpStatement();
	bool DeclarationSpecifier();
	void Declaration();
	void TranslationUnit();

private:
	Lexer lex;
	AST stree;
	Token m_currentToken;
	Token m_lastToken;
	std::stack<std::unique_ptr<ASTNode>> m_elementStack;
	std::shared_ptr<Value> m_currentData = nullptr;
	std::shared_ptr<Value> m_lastData = nullptr;
};

