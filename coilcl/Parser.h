#pragma once

#include "Lexer.h"
#include "AST.h"

#include <stack>

class TokenState
{
	Token m_currentToken;
	std::shared_ptr<Value> m_currentData = nullptr;

public:
	TokenState(const TokenState& rhs)
		: m_currentToken{ rhs.m_currentToken }
		, m_currentData{ rhs.m_currentData }
	{
	}

	TokenState(Token currentToken, std::shared_ptr<Value> currentData)
		: m_currentToken{ currentToken }
		, m_currentData{ currentData }
	{
	}

	TokenState(int currentToken, std::shared_ptr<Value> currentData)
		: m_currentToken{ static_cast<Token>(currentToken) }
		, m_currentData{ currentData }
	{
	}

	Token FetchToken()
	{
		return m_currentToken;
	}
};

template<typename T>
class Command
{
	std::stack<size_t> m_snapshopList;
	std::vector<T> m_tokenList;
	size_t index = 0;

public:
	Command()
	{
		m_tokenList.reserve(10);
	}

	inline void Push(const T& state)
	{
		m_tokenList.push_back(state);
		index++;
	}

	inline auto Previous()
	{
		//TODO: out of bounds exception
		return m_tokenList.at(index - 1);
	}

	inline auto Current()
	{
		return m_tokenList[index - 1];
	}

	inline auto operator[](size_t idx)
	{
		return m_tokenList[idx];
	}

	void Clear()
	{
		m_tokenList.clear();
		while (!m_snapshopList.empty()) {
			m_snapshopList.pop();
		}
	}
};

class Parser
{

public:
	Parser(const std::string& input);
	void Execute();

protected:
	void Error(const std::string& err);
	void ExpectToken(Token token);
	void ExpectIdentifier();

	/*inline auto LastToken() { return m_comm.Previous().FetchToken(); }
	inline auto CurrentToken() { return m_comm.Current().FetchToken(); }*/

	inline void NextToken()
	{
		m_comm.Push(TokenState(lex.Lex(), lex.HasData() ? std::shared_ptr<Value>(std::move(lex.Data())) : nullptr));

		/*m_lastToken = m_currentToken;
		m_currentToken = static_cast<Token>(lex.Lex());

		if (lex.HasData()) {
			m_lastData = std::move(m_currentData);
			m_currentData = std::move(lex.Data());
		}*/
	}

private:
	auto StorageClassSpecifier();
	auto TypeQualifier();
	std::unique_ptr<Value> TypeSpecifier();
	bool DeclarationSpecifiers();
	void StructOrUnionSpecifier();
	void StructDeclarationList();
	void SpecifierQualifierList();
	void StructDeclaratorList();
	void EnumSpecifier();
	void EnumeratorList();
	bool UnaryOperator();
	bool AssignmentOperator();

private: // Expressions
	void PrimaryExpression();
	void ArgumentExpressionList();
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
	void TypeName();
	void AbstractDeclarator();
	void DirectAbstractDeclarator();
	void Initializer();
	void InitializerList();
	void Designation();
	void Designators();
	void Pointer();
	bool Declarator();
	bool DirectDeclarator();
	void TypeQualifierList();
	bool ParameterTypeList();
	bool ParameterDeclaration();

private:
	bool FunctionDefinition();
	void ExternalDeclaration();
	void TranslationUnit();

private:
	Lexer lex;
	AST stree;
	//Token m_currentToken;
	//Token m_lastToken;
	Command<TokenState> m_comm;
	std::stack<std::unique_ptr<ASTNode>> m_elementStack;
	std::unique_ptr<Value> m_currentData = nullptr;
	//std::unique_ptr<Value> m_lastData = nullptr;
};

