#pragma once

#include "Profile.h"
#include "Lexer.h"
#include "AST.h"
#include "SuperStack.h"

#include <deque>
#include <stack>

class TokenState
{
	Token m_currentToken;
	std::shared_ptr<Value> m_currentData = nullptr;
	int m_line;
	int m_column;

public:
	TokenState(Token currentToken, std::shared_ptr<Value>& currentData, int line = 0, int column = 0)
		: m_currentToken{ currentToken }
		, m_currentData{ currentData }
		, m_line{ line }
		, m_column{ column }
	{
	}

	TokenState(int currentToken, std::shared_ptr<Value>& currentData, int line = 0, int column = 0)
		: m_currentToken{ static_cast<Token>(currentToken) }
		, m_currentData{ currentData }
		, m_line{ line }
		, m_column{ column }
	{
	}

	TokenState(int currentToken, std::shared_ptr<Value>& currentData, std::pair<int, int>&& location)
		: m_currentToken{ static_cast<Token>(currentToken) }
		, m_currentData{ currentData }
		, m_line{ location.first }
		, m_column{ location.second }
	{
	}

	TokenState(const TokenState& other) = default;
	TokenState(TokenState&& other) = default;

	inline bool HasData() const { return !!m_currentData; }

	inline const std::shared_ptr<Value>& FetchData() { return m_currentData; }

	inline auto FetchToken() const { return m_currentToken; }
	inline auto FetchLine() const { return m_line; }
	inline auto FetchColumn() const { return m_column; }
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

	// Prevent all forms of data transfer from this object
	StateContainer(const StateContainer&) = delete;
	StateContainer(StateContainer&&) = delete;
	StateContainer& operator=(const StateContainer&) = delete;
	StateContainer& operator=(StateContainer&&) = delete;

	void Push(T&& state)
	{
		m_tokenList.push_back(std::move(state));
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

	inline void Dispose()
	{
		m_snapshopList.pop();
	}

	inline auto IsIndexHead() const
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
	Parser(std::shared_ptr<Compiler::Profile>& profile);
	Parser& Execute();

	std::shared_ptr<TranslationUnitDecl> DumpAST() const
	{
		return m_ast;
	}

protected:
	void Error(const char *err);
	void ExpectToken(Token token);
	void ExpectIdentifier();

	void NextToken()
	{
		if (m_comm.IsIndexHead()) {
			auto itok = lex.Lex();
			auto location = std::make_pair(lex.TokenLine(), lex.TokenColumn());
			auto val = lex.HasData() ? std::shared_ptr<Value>(std::move(lex.Data())) : nullptr;
			m_comm.Push(TokenState(itok, val, std::move(location)));
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
	bool JumpStatement();
	bool LabeledStatement();
	bool CompoundStatement();
	void ExpressionStatement();
	bool SelectionStatement();
	bool IterationStatement();
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
	std::shared_ptr<TranslationUnitDecl> m_ast;
	StateContainer<TokenState> m_comm;
	std::deque<std::shared_ptr<ASTNode>> m_elementStack;
	std::shared_ptr<Compiler::Profile> m_profile;

	std::stack<std::string> m_identifierStack;
	//std::extension::super_stack<std::string> m_identifierInline;
};

