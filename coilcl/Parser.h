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

	inline bool HasData() const
	{
		return !!m_currentData;
	}

	inline std::shared_ptr<Value>& FetchData()
	{
		return m_currentData;
	}

	inline auto FetchToken() const
	{
		return m_currentToken;
	}
};

template<typename T>
class StateContainer
{
	std::stack<size_t> m_snapshopList;
	std::vector<T> m_tokenList;
	size_t index = 0;

public:
	StateContainer(size_t reserved_elements = 10)
	{
		m_tokenList.reserve(reserved_elements);
	}

	StateContainer(const StateContainer&) = delete;
	StateContainer(StateContainer&&) = delete;
	StateContainer& operator=(const StateContainer&) = delete;

	inline void Push(const T& state)
	{
		m_tokenList.push_back(state);
		index++;
	}

	inline auto Previous()
	{
		//TODO: out of bounds exception
		return m_tokenList[index - 1];
	}

	inline auto Current()
	{
		return m_tokenList[index - 1];
	}

	inline void Snapshot()
	{
		m_snapshopList.push(index);
	}

	inline void Revert()
	{
		index = m_snapshopList.top();
		m_snapshopList.pop();
	}

	inline bool IsIndexHead() const
	{
		return index == m_tokenList.size();
	}

	inline void ShiftForward()
	{
		++index;
	}

	inline auto operator[](size_t idx)
	{
		return m_tokenList[idx];
	}

	void Reset()
	{
		index = m_tokenList.size();
		while (!m_snapshopList.empty()) {
			m_snapshopList.pop();
		}
	}

	void Undo()
	{
		if (index > 0) {
			--index;
		}
	}

	// Clear the token and snapshot list, this action will free all allocated memory
	void Clear()
	{
		index = 0;
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
	void Error(const char *err);
	void ExpectToken(Token token);
	void ExpectIdentifier();

	void NextToken()
	{
		if (m_comm.IsIndexHead()) {
			auto itok = lex.Lex();
			auto val = lex.HasData() ? std::shared_ptr<Value>(std::move(lex.Data())) : nullptr;
			m_comm.Push(TokenState(itok, val));
		}
		else {
			m_comm.ShiftForward();
		}
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
	StateContainer<TokenState> m_comm;
	std::stack<std::unique_ptr<ASTNode>> m_elementStack;
};

