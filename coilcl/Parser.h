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
			m_lastData = std::move(m_currentData);
			m_currentData = std::move(lex.Data());
		}
	}

private:
	auto StorageClassSpecifier();
	auto TypeQualifier();
	std::unique_ptr<Value> TypeSpecifier();
	bool DeclarationSpecifiers();
	bool UnaryOperator();
	bool AssignmentOperator();

private: // Expressions
	void PrimaryExpression();
	void PostfixExpression();
	void UnaryExpression();
	void CastExpression();
	void MultiplicativeExpression();
	void AdditiveExpression();
	void ShiftExpression();
	void RelationalExpression();
	void EqualityExpression();
	void AndExpression();
	void ExclusiveOrExpression();
	void LogicalAndExpression();
	void LogicalOrExpression();
	void ConditionalExpression();
	void AssignmentExpression();
	void Expression();
	void ConstantExpression();

private: // Statements
	void JumpStatement();
	void LabeledStatement();
	bool CompoundStatement();
	void ExpressionStatement();
	void SelectionStatement();
	void IterationStatement();
	void Statement();

private: // Declarations
	void BlockItems();
	void Declaration();
	void InitDeclaratorList();
	void InitDeclarator();
	void Initializer();
	void InitializerList();
	void Designation();
	void Designators();
	void Pointer();
	bool Declarator();
	bool DirectDeclarator();

private:
	bool FunctionDefinition();
	void ExternalDeclaration();
	void TranslationUnit();

private:
	Lexer lex;
	AST stree;
	Token m_currentToken;
	Token m_lastToken;
	std::stack<std::unique_ptr<ASTNode>> m_elementStack;
	std::unique_ptr<Value> m_currentData = nullptr;
	std::unique_ptr<Value> m_lastData = nullptr;
};

