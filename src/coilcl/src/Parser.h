// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "Profile.h"
#include "Tokenizer.h"
#include "Lexer.h"

#include <CryCC/AST.h>
#include <CryCC/Program.h>

#include <Cry/Functional.h>
#include <Cry/LockPipe.h>

#include <boost/optional.hpp>

#include <deque>
#include <stack>
#include <map>

namespace Typedef = CryCC::SubValue::Typedef;
namespace Valuedef = CryCC::SubValue::Valuedef;

// Pop all stack values as long as the stack contains elements.
template<typename Type, typename = typename std::enable_if<Cry::Functional::IsStack<Type>::value>::type>
inline void ClearStack(Type& c)
{
	while (!c.empty()) { c.pop(); }
}

//TODO: replace token & value with tokenDataPair
class TokenState
{
	Token m_currentToken;
	boost::optional<Valuedef::Value> m_currentData;
	int m_line{ 0 }; //TODO: remplac with location thing
	int m_column{ 0 };  //TODO: remplac with location thing

public:
	using TokenType = decltype(m_currentToken);
	using ValueType = decltype(m_currentData)::value_type;

public:
	TokenState(Token currentToken)
		: m_currentToken{ currentToken }
	{
	}

	TokenState(Token currentToken, Valuedef::Value& currentData, int line = 0, int column = 0)
		: m_currentToken{ currentToken }
		, m_currentData{ currentData }
		, m_line{ line }
		, m_column{ column }
	{
	}

	TokenState(Token currentToken, Valuedef::Value&& currentData, std::pair<int, int>&& location)
		: m_currentToken{ currentToken }
		, m_currentData{ std::move(currentData) }
		, m_line{ location.first }
		, m_column{ location.second }
	{
	}

	TokenState(const TokenState& other) = default;
	TokenState(TokenState&& other) = default;

	// Test if current token state contains data
	inline bool HasData() const { return (!!m_currentData); }

	// Fetch data from current token state
	inline const Valuedef::Value& FetchData() { return m_currentData.get(); }

	// Fetch token from current token state
	inline auto FetchToken() const { return m_currentToken; }

	// Fetch source line from current token state
	inline auto FetchLine() const { return m_line; }

	// Fetch source column from current token state
	inline auto FetchColumn() const { return m_column; }

	// Fetch source location as pair
	inline auto FetchLocation() const { return std::make_pair(m_line, m_column); }
};

//TODO: iterator, check StateType
template<typename StateType>
class StateContainer
{
	using OffsetType = size_t;

	std::stack<size_t> m_snapshopList;
	std::vector<StateType> m_tokenList;
	mutable OffsetType index{ 0 };

public:
	StateContainer(size_t reserved_elements = 10)
	{
		m_tokenList.reserve(reserved_elements);
	}

	//
	// Prevent transfer from this object.
	//

	StateContainer(const StateContainer&) = delete;
	StateContainer(StateContainer&&) = delete;
	StateContainer& operator=(const StateContainer&) = delete;
	StateContainer& operator=(StateContainer&&) = delete;

	// Push state on the list.
	void Push(StateType&& state)
	{
		m_tokenList.push_back(std::move(state));
		++index;
	}

	// Emplace state at the end of list.
	template<typename... ArgTypes>
	void Emplace(ArgTypes&&... args)
	{
		m_tokenList.emplace_back(std::forward<ArgTypes>(args));
		++index;
	}

	// Get previous state.
	inline auto& Previous() { return m_tokenList[index > 1 ? (index - 1) : index]; }
	// Get current state.
	inline auto& Current() { return m_tokenList[index - 1]; }

	// Take snapshot of current index.
	inline void Snapshot()
	{
		m_snapshopList.push(index);
	}

	// Revert to last snapshot.
	inline void Revert()
	{
		index = m_snapshopList.top();
		m_snapshopList.pop();
	}

	// Dispose last snapshot.
	inline void DisposeSnapshot()
	{
		m_snapshopList.pop();
	}

	// Check if snapshots exist.
	inline bool HasSnapshots() const noexcept { return !m_snapshopList.empty(); }

	// Check if the next item is the last item.
	inline auto IsIndexHead() const { return index == m_tokenList.size(); }

	// Take one step forward.
	inline void ShiftForward() const
	{
		if (m_tokenList.size() > index) {
			++index;
		}
	}

	// Take one step back, but preserve the list.
	inline void ShiftBackward() const
	{
		if (index > 0) {
			--index;
		}
	}

	// Access token at random position.
	inline auto& operator[](size_t idx) const
	{
		return m_tokenList[idx];
	}

	// Remove all snapshots, and restore index.
	void Reset()
	{
		index = m_tokenList.size();
		ClearStack(m_snapshopList);
	}

	// Get container size.
	inline size_t Size() const noexcept { return m_tokenList.size(); }
	// Check if container is empty.
	inline bool Empty() const noexcept { return m_tokenList.empty(); }

	// Clear the token and snapshot list, this action will free all allocated memory.
	void Clear()
	{
		index = 0;
		m_tokenList.clear();
		ClearStack(m_snapshopList);
	}

	// If the next item is the last item, clear the list
	// and copy the last element back in the container.
	void TryClear()
	{
		if (IsIndexHead()) {
			auto currentItem = Current();
			Clear();
			Push(std::move(currentItem));
		}
	}
};

struct CompareStringPair
{
	using type = std::pair<std::string, int>;

	auto operator()(const type& a, const type& b) const
	{
		return a.first < b.first;
	}
};

class Parser : public CryCC::Program::Stage<Parser>
{
public:
	Parser(std::shared_ptr<CoilCl::Profile>& profile, CoilCl::TokenizerPtr tokenizer, CryCC::Program::ConditionTracker::Tracker&);

	std::string Name() const { return "Parser"; }

	Parser& Execute();
	Parser& CheckCompatibility();

	// Dump AST to program structure
	std::shared_ptr<TranslationUnitDecl> DumpAST() const
	{
		if (m_ast == nullptr) {
			throw StageBase::StageException{ Name(), "abstract program is empty" };
		}

		return m_ast;
	}

protected:
	void Error(const char *err, Token token);
	void ExpectToken(Token token);
	void ExpectIdentifier();
	void NextToken();

private:
	auto StorageClassSpecifier();
	auto TypeQualifier();
	bool TypeSpecifier();
	bool DeclarationSpecifiers();
	bool TypenameSpecifier();
	bool StructOrUnionSpecifier();
	void SpecifierQualifierList();
	bool EnumSpecifier();
	bool UnaryOperator();
	bool AssignmentOperator();
	void CompoundLiteral();

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
	void InclusiveOrExpression();
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
	void Designation();
	void Designators();
	bool IdentifierListDecl();
	void Pointer();
	bool Declarator();
	bool DirectDeclarator();
	void TypeQualifierList();
	bool PostParameterTypeList();
	bool ParameterTypeList();
	bool ParameterDeclaration();

private:
	bool FunctionDefinition();
	void ExternalDeclaration();
	void TranslationUnit();

private:
	CoilCl::TokenizerPtr lex;
	std::shared_ptr<TranslationUnitDecl> m_ast;
	StateContainer<TokenState> m_comm;
	std::shared_ptr<CoilCl::Profile> m_profile;

	// Temporary parser containers.
	size_t m_pointerCounter = 0;
	std::map<std::pair<std::string, int>, std::shared_ptr<RecordDecl>, CompareStringPair> m_recordList;
	std::map<std::string, std::shared_ptr<Typedef::TypedefBase>> m_typedefList;
	std::stack<std::shared_ptr<Typedef::TypedefBase>> m_typeStack;
	std::stack<std::string> m_identifierStack;
	Cry::LockPipe<std::shared_ptr<CryCC::AST::ASTNode>> m_elementDescentPipe;
};
