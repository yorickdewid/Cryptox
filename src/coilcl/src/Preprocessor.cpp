// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be
// copied and/or distributed without the express of the author.

//FUTURE:
// - Macro expansion

#include "Cry/Indep.h"

#include "Preprocessor.h"
#include "DirectiveScanner.h" //TODO: remove, only used for tokens
#include "IntrusiveScopedPtr.h"

#include <boost/logic/tribool.hpp>

#include <set>
#include <stack>
#include <cassert>
#include <iostream>

using namespace CoilCl;

static std::map<std::string, std::vector<Preprocessor::TokenDataPair<TokenProcessor::TokenType, const TokenProcessor::DataType>>> g_definitionList;
static std::set<std::string> g_sourceGuardList;

namespace Cry
{
namespace Algorithm
{

template<typename _Ty, typename _KeyTy, class _Predicate>
auto ForEachRangeEqual(_Ty& set, _KeyTy& key, _Predicate p)
{
	auto range = set.equal_range(key);
	for (auto it = range.first; it != range.second; it++) {
		if (p(it->second)) {
			return it;
		}
	}

	return set.end();
}

} // namespace Algorithm
} // namespace Cry

static class TokenSubscription
{
public:
	using CallbackFunc = void(*)(bool, Preprocessor::DefaultTokenDataPair&);

public:
	// Register callback for token, but only if token callback pair does not exist
	void SubscribeOnToken(int token, CallbackFunc cb)
	{
		const auto& range = m_subscriptionTokenSet.equal_range(token);
		bool isRegistered = std::any_of(range.first, range.second, [&cb](std::multimap<int, CallbackFunc>::value_type pair)
		{
			return pair.second == cb;
		});

		if (isRegistered) { return; }

		m_subscriptionTokenSet.emplace(token, cb);
	}

	// Find token and callback, then erase from set
	void UnsubscribeOnToken(int token, CallbackFunc cb)
	{
		auto it = Cry::Algorithm::ForEachRangeEqual(m_subscriptionTokenSet, token, [&cb](CallbackFunc& _cb)
		{
			return _cb == cb;
		});

		if (it != m_subscriptionTokenSet.end()) {
			m_subscriptionTokenSet.erase(it);
		}
	}

	// Register any calls that trigger on each token
	void SubscribeOnAll(CallbackFunc cb)
	{
		m_subscriptionSet.emplace(cb);
	}

	// Find the callback, and erase from set
	void UnsubscribeOnAll(CallbackFunc cb)
	{
		m_subscriptionSet.erase(cb);
	}

	// Invoke all callbacks for this token
	void CallAnyOf(bool isDirective, Preprocessor::DefaultTokenDataPair& tokenData)
	{
		auto range = m_subscriptionTokenSet.equal_range(tokenData.Token());
		for (auto& it = range.first; it != range.second; ++it) {
			(it->second)(isDirective, tokenData);
		}

		// Invoke any callback function that was registered for all tokens
		auto it = m_subscriptionSet.begin();
		while (it != m_subscriptionSet.end()) {
			(*it++)(isDirective, tokenData);
		}
	}

private:
	std::multimap<int, CallbackFunc> m_subscriptionTokenSet;
	std::set<CallbackFunc> m_subscriptionSet;
} g_tokenSubscription;

void RegisterStandardMacros()
{
	//TODO:

	//TODO: __func__
	//g_definitionList.insert({ "__FILE__", nullptr });
	//g_definitionList.insert({ "__LINE__", nullptr });
	//g_definitionList.insert({ "__DATE__", nullptr }); // "??? ?? ????"
	//g_definitionList.insert({ "__TIME__", nullptr }); // "??:??:??"
	//g_definitionList.insert({ "__STDC__", nullptr });
	//g_definitionList.insert({ "__STDC_VERSION__", nullptr });
	//g_definitionList.insert({ "__STDC_HOSTED__", nullptr });
	//g_definitionList.insert({ "__STDC_HOSTED__", nullptr });
}

void RegisterCommonMacros()
{
	//TODO: __VERSION__
}

Preprocessor::Preprocessor(std::shared_ptr<CoilCl::Profile>& profile)
	: Stage{ this }
	, m_profile{ profile }
{
	RegisterStandardMacros();
	RegisterCommonMacros();
}

Preprocessor& Preprocessor::CheckCompatibility()
{
	return (*this);
}

namespace CoilCl
{
namespace LocalMethod
{

class AbstractDirective
{
public:
	AbstractDirective() = default;
	virtual void Dispence(TokenProcessor::TokenType token, const TokenProcessor::DataType data) = 0;

protected:
	class DirectiveException : public std::runtime_error
	{
	public:
		DirectiveException(const std::string& message) noexcept
			: std::runtime_error{ message.c_str() }
		{
		}

		explicit DirectiveException(const std::string& directive, const std::string& message) noexcept
			: std::runtime_error{ (directive + ": " + message).c_str() }
		{
		}
	};

	class UnexpectedTokenException : public DirectiveException
	{
	public:
		UnexpectedTokenException(const std::string& message) noexcept
			: DirectiveException{ message }
		{
		}
	};

	template<typename _Ty>
	static inline _Ty ConvertDataAs(const TokenProcessor::DataType& data)
	{
		return data->As<_Ty>();
	}

	void RequireToken(int expectedToken, int token)
	{
		//TODO:
		if (expectedToken != token) {
			throw UnexpectedTokenException{ "expected token" };
		}
	}

	void RequireData(const TokenProcessor::DataType& data)
	{
		// Data was expected, throw if not found
		if (!data) {
			throw DirectiveException{ "expected constant" };
		}
	}
};

class ImportSource : public AbstractDirective
{
	bool hasBegin = false;
	std::string tempSource;

	// Request input source push from the frontend
	void Import(const std::string& source)
	{
		std::cout << "import " << source << std::endl;

		//TODO: call push import sources
	}

public:
	void Dispence(TokenProcessor::TokenType token, const TokenProcessor::DataType data)
	{
		switch (token) {
		case TK_LESS_THAN: // Global includes begin
			hasBegin = true;
			break;
		case TK_GREATER_THAN: // Global includes end
			if (!hasBegin) throw;
			Import(tempSource);
			break;
		case TK_CONSTANT: // Local include
			RequireData(data);
			Import(ConvertDataAs<std::string>(data));
			break;
		default:
			if (hasBegin) {
				if (!data) {
					//TODO: Feed token back to generator
					//TODO: This list is my no means complete
					switch (token) {
					case TK_DOT:
						tempSource.push_back('.');
						break;
					case TK_COMMA:
						tempSource.push_back(',');
					}
					break;
				}

				tempSource.append(ConvertDataAs<std::string>(data));
				break;
			}

			// Did not match the parsing pattern, throw
			throw DirectiveException{ "include", "expected constant or '<' after 'include'" };
		}
	}
};

// Definition and expansion
class DefinitionTag : public AbstractDirective
{
	std::string m_definitionName;
	std::vector<Preprocessor::TokenDataPair<TokenProcessor::TokenType, const TokenProcessor::DataType>> m_definitionBody;

public:
	void Dispence(TokenProcessor::TokenType token, const TokenProcessor::DataType data)
	{
		// First item must be the definition name
		if (m_definitionName.empty()) {
			RequireData(data);
			const auto definitionName = ConvertDataAs<std::string>(data);
			if (g_definitionList.find(definitionName) != g_definitionList.end()) {
				throw DirectiveException{ "define", "'" + definitionName + "' already defined" };
			}

			m_definitionName = definitionName;
			return;
		}

		// Save token with optional data on the vector
		m_definitionBody.push_back({ token, data });
	}

	// If the data matches a definition in the global definition list, replace it
	static void OnPropagateCallback(bool isDirective, Preprocessor::DefaultTokenDataPair& dataPair)
	{
		using namespace Valuedef;
		using namespace Typedef;

		// Do not interfere with preprocessor lines
		if (isDirective) { return; }

		auto it = g_definitionList.find(ConvertDataAs<std::string>(dataPair.Data()));
		if (it == g_definitionList.end()) { return; }

		// Definition without body, reset all
		if (it->second.empty()) {
			dataPair.ResetToken();
			dataPair.ResetData();
			return;
		}

		dataPair.AssignToken(it->second.at(0).Token());
		dataPair.AssignData(it->second.at(0).Data());

		// When multiple tokens are registered for this definition, create a token queue
		if (it->second.size() > 1) {
			auto dequqPtr = std::make_unique<std::deque<decltype(g_definitionList)::mapped_type::value_type>>();
			for (auto subit = it->second.begin() + 1; subit != it->second.end(); ++subit) {
				dequqPtr->push_back(std::move((*subit)));
			}

			dataPair.EmplaceTokenQueue(std::move(dequqPtr));
		}
	}

	~DefinitionTag()
	{
		if (m_definitionName.empty()) { return; }

		//FUTURE: OPTIMIZATION: Try intergral evaluation before move
		// Insert definition body into global definition list
		const auto& result = g_definitionList.insert({ m_definitionName, std::move(m_definitionBody) });
		assert(result.second);

		// Subscribe on all identifier tokens apart from the define tag. This indicates
		// we can do work on 'normal' tokens that would otherwise flow directly through to
		// the caller frontend. In this case we register identifier to examine, and on match
		// replace with the corresponding value. This essentially allows for find and replace
		// semantics on the provided input file before any other stage has seen the token stream.
		g_tokenSubscription.SubscribeOnToken(TK_IDENTIFIER, &DefinitionTag::OnPropagateCallback);
	}
};

// Remove definition from list
class DefinitionUntag : public AbstractDirective
{
public:
	void Dispence(TokenProcessor::TokenType token, const TokenProcessor::DataType data)
	{
		CRY_UNUSED(token);

		RequireData(data);
		auto it = g_definitionList.find(ConvertDataAs<std::string>(data));
		if (it == g_definitionList.end()) { return; }

		// Remove definition from global define list
		g_definitionList.erase(it);
	}
};

// Conditional compilation
class ConditionalStatement : public AbstractDirective
{
	static std::stack<bool> evaluationResult;
	std::vector<Preprocessor::TokenDataPair<TokenProcessor::TokenType, const TokenProcessor::DataType>> m_statementBody;

	class ConditionalStatementException : public std::runtime_error
	{
	public:
		ConditionalStatementException(const std::string& message) noexcept
			: std::runtime_error{ message.c_str() }
		{
		}
	};

	// Evaluate statemenet and return either true for positive
	// result, or false for negative. An evaluation error will
	// throw an exception.
	static bool Eval(std::vector<Preprocessor::TokenDataPair<TokenProcessor::TokenType, const TokenProcessor::DataType>>&& statement)
	{
		class ChainAction
		{
			boost::logic::tribool chainState{ boost::logic::indeterminate };

		public:
			bool conjunction : 1;
			bool disjunction : 1;
			bool negation : 1;

			ChainAction() { Reset(); }

			// Reset all actions
			void Reset()
			{
				conjunction = false;
				disjunction = false;
				negation = false;
			}

			// Return evaluated expression as boolean
			inline bool Consensus() const noexcept
			{
				assert(!boost::logic::indeterminate(chainState));
				return chainState;
			}

			// Boolean arithmetic
			void Consolidate(bool b)
			{
				if (negation) { b = !b; }
				if (conjunction) {
					assert(!boost::logic::indeterminate(chainState));
					chainState = chainState && b;
				}
				else if (disjunction) {
					assert(!boost::logic::indeterminate(chainState));
					chainState = chainState || b;
				}
				else { chainState = b; }

				Reset();
			}
		} consensusAction;

		// Two value stack represents lhs and rhs
		int stack[2] = { 0,0 };

		for (auto it = statement.begin(); it != statement.end(); ++it) {
			switch (it->Token()) {
			case TK_IDENTIFIER:
			case TK_CONSTANT:
			{
				assert(it->HasData());
				switch (it->Data()->DataType<CoilCl::Typedef::BuiltinType>()->TypeSpecifier()) {
				case CoilCl::Typedef::BuiltinType::Specifier::INT:
				{
					stack[0] = it->Data()->As<int>();
					consensusAction.Consolidate(stack[0]);
					break;
				}
				case CoilCl::Typedef::BuiltinType::Specifier::CHAR:
				{
					const std::string definition = it->Data()->IsArray()
						? it->Data()->As<std::string>()
						: std::string{ it->Data()->As<char>() };
					bool hasDefinition = g_definitionList.find(definition) != g_definitionList.end();
					consensusAction.Consolidate(hasDefinition);
					break;
				}
				default:
					throw ConditionalStatementException{ "invalid constant in preprocessor expression" };
				}

				continue;
			}

			case TK_PARENTHESE_OPEN:
			case TK_PARENTHESE_CLOSE:
				continue;

				// defined statement
			case TK_PP_DEFINED:
			{
				++it;
				if (it->Token() == TK_PARENTHESE_OPEN) {
					++it;
					if (it->Token() != TK_IDENTIFIER) {
						throw ConditionalStatementException{ "expected identifier" };
					}
					assert(it->HasData());
					const std::string definition = it->Data()->As<std::string>();
					++it;
					if (it->Token() != TK_PARENTHESE_CLOSE) {
						throw ConditionalStatementException{ "expected )" };
					}
					consensusAction.Consolidate(g_definitionList.find(definition) != g_definitionList.end());
					continue;
				}
				if (it->Token() != TK_IDENTIFIER) {
					throw ConditionalStatementException{ "expected identifier" };
				}
				assert(it->HasData());
				const std::string definition = it->Data()->As<std::string>();
				consensusAction.Consolidate(g_definitionList.find(definition) != g_definitionList.end());
				continue;
			}

			// Logical operators
			case TK_AND_OP:
			{
				consensusAction.conjunction = true;
				continue;
			}
			case TK_OR_OP:
			{
				consensusAction.disjunction = true;
				continue;
			}
			case TK_NOT:
			{
				consensusAction.negation = true;
				continue;
			}

#define COMPARE_OP(o) \
			++it; \
			if (it->Token() != TK_CONSTANT) {  throw ConditionalStatementException{ "expected constant" }; } \
			assert(it->HasData()); \
			stack[1] = it->Data()->As<int>(); \
			consensusAction.Consolidate(stack[0] o stack[1]); \
			stack[0] = 0; stack[1] = 0;

			// Comparison operators
			case TK_GREATER_THAN:
			{
				COMPARE_OP(> );
				continue;
			}
			case TK_LESS_THAN:
			{
				COMPARE_OP(< );
				continue;
			}
			case TK_EQ_OP:
			{
				COMPARE_OP(== );
				continue;
			}
			case TK_GE_OP:
			{
				COMPARE_OP(>= );
				continue;
			}
			case TK_LE_OP:
			{
				COMPARE_OP(<= );
				continue;
			}
			case TK_NE_OP:
			{
				COMPARE_OP(!= );
				continue;
			}

#define ARITHMETIC_OP(o) \
			++it; \
			if (it->Token() != TK_CONSTANT) { throw ConditionalStatementException{ "expected constant" }; } \
			assert(it->HasData()); \
			stack[0] = stack[0] o it->Data()->As<int>();

			// Integral arithmetic
			case TK_PLUS:
			{
				ARITHMETIC_OP(+);
				continue;
			}
			case TK_MINUS:
			{
				ARITHMETIC_OP(-);
				continue;
			}
			case TK_ASTERISK:
			{
				ARITHMETIC_OP(*);
				continue;
			}
			case TK_SLASH:
			{
				ARITHMETIC_OP(/ );
				continue;
			}

			//FUTURE: %, ~, ^

			default:
				throw ConditionalStatementException{ "invalid token in preprocessor directive" };
			}
		}

		return consensusAction.Consensus();
	}

public:
	ConditionalStatement() = default;

	template<typename _TokenTy>
	ConditionalStatement(_TokenTy token)
	{
		m_statementBody.push_back({ token, nullptr });
	}

	template<typename _TokenTy, typename... _ArgsTy>
	ConditionalStatement(_TokenTy token, _ArgsTy... args)
		: ConditionalStatement{ args... }
	{
		m_statementBody.push_back({ token, nullptr });
	}

	void Dispence(TokenProcessor::TokenType token, const TokenProcessor::DataType data)
	{
		m_statementBody.push_back({ token, data });
	}

	~ConditionalStatement()
	{
		if (m_statementBody.empty()) {
			//throw ConditionalStatementException{ "statement with no expression" };
			// Ouch!
		}

		// Evaluate the statement and push the boolean result on the stack
		evaluationResult.push(Eval(std::move(m_statementBody)));
		g_tokenSubscription.SubscribeOnAll(&ConditionalStatement::OnPropagateCallback);
	}

	static void OnPropagateCallback(bool isDirective, Preprocessor::DefaultTokenDataPair& dataPair)
	{
		CRY_UNUSED(isDirective);

		// If this token happens to be a conditional statement token, redirect
		switch (dataPair.Token()) {
		case TK_IF: { return; };
		case TK_PP_ELIF: { EncounterElseIf(); return; };
		case TK_ELSE: { EncounterElse(); dataPair.ResetToken(); return; };
		case TK_PP_ENDIF: { EncounterEndif(); dataPair.ResetToken(); return; }
		}

		// If the evaluation stack is empty, or the top item is true, bail
		if (evaluationResult.empty() || evaluationResult.top()) { return; }

		// Resetting the token indicates the proxy must skip the token
		dataPair.ResetToken();
	}

	static void EncounterElseIf()
	{
		if (evaluationResult.empty()) {
			throw ConditionalStatementException{ "unexpected elif" };
		}

		evaluationResult.pop();
		g_tokenSubscription.UnsubscribeOnAll(&ConditionalStatement::OnPropagateCallback);
	}

	// Flip evaluation result
	static void EncounterElse()
	{
		if (evaluationResult.empty()) {
			throw ConditionalStatementException{ "unexpected else" };
		}

		// Inverse top most element
		auto& top = evaluationResult.top();
		top = !top;
	}

	// End of if statement
	static void EncounterEndif()
	{
		if (evaluationResult.empty()) {
			throw ConditionalStatementException{ "unexpected endif" };
		}

		// Pop top item from evaluation stack. If this happens to be the 
		// last item on the stack also unregister the callback subscription.
		evaluationResult.pop();
		if (evaluationResult.empty()) {
			g_tokenSubscription.UnsubscribeOnAll(&ConditionalStatement::OnPropagateCallback);
		}
	}
};

std::stack<bool> ConditionalStatement::evaluationResult;

// Parse compiler pragmas
class CompilerDialect : public AbstractDirective
{
	const std::array<std::string, 1> trivialToken = { "once" };

	bool HandleTrivialCase(const std::string& identifier)
	{
		if (!std::any_of(trivialToken.cbegin(), trivialToken.cend(), [&identifier](const std::string& tok)
		{
			return tok == identifier;
		})) {
			return false;
		}

		// In case of 'once' fetch the source name and add to source
		// guard list. If the source was already on the guard list, return
		// imediatly and skip this source.
		if (identifier == trivialToken[0]) {
			auto result = g_sourceGuardList.emplace("somefile.c");
			if (result.second) {
				//TODO: bail by exception
			}
		}

		return true;
	}

public:
	void Dispence(TokenProcessor::TokenType token, const TokenProcessor::DataType data)
	{
		RequireToken(TK_IDENTIFIER, token);
		if (HandleTrivialCase(ConvertDataAs<std::string>(data))) { return; }
	}
};

// Set source location to fixed line,col pair
class FixLocation : public AbstractDirective
{
public:
	void Dispence(TokenProcessor::TokenType token, const TokenProcessor::DataType data)
	{
		CRY_UNUSED(token);
		CRY_UNUSED(data);

		//TODO: We have no clue what to do with #line
	}
};

// Report linquistic error
class LinguisticError : public AbstractDirective
{
	const bool m_isFatal;

public:
	LinguisticError(bool fatal = true)
		: m_isFatal{ fatal }
	{
	}

	void Dispence(TokenProcessor::TokenType token, const TokenProcessor::DataType data)
	{
		if (token != TK_CONSTANT) {
			throw DirectiveException{ "error", "expected constant after 'error'" };
		}

		// Throw if message was fatal, this wil halt all operations
		if (m_isFatal) {
			throw DirectiveException{ ConvertDataAs<std::string>(data) };
		}
	}
};

} // namespace LocalMethod
} // namespace CoilCl

template<typename _Ty, typename... _ArgsTy>
auto MakeMethod(_ArgsTy... args) -> std::shared_ptr<_Ty>
{
	return std::make_shared<_Ty>(std::forward<_ArgsTy>(args)...);
}

void Preprocessor::MethodFactory(TokenType token)
{
	using namespace ::LocalMethod;

	switch (token) {
	case TK_PP_INCLUDE:
		m_method = MakeMethod<ImportSource>();
		break;
	case TK_PP_DEFINE:
		m_method = MakeMethod<DefinitionTag>();
		break;
	case TK_PP_UNDEF:
		m_method = MakeMethod<DefinitionUntag>();
		break;
	case TK_IF:
		m_method = MakeMethod<ConditionalStatement>();
		break;
	case TK_PP_IFDEF:
		m_method = MakeMethod<ConditionalStatement>(TK_PP_DEFINED);
		break;
	case TK_PP_IFNDEF:
		m_method = MakeMethod<ConditionalStatement>(TK_PP_DEFINED, TK_NOT);
		break;
	case TK_PP_ELIF:
		m_method = MakeMethod<ConditionalStatement>();
		break;
	case TK_PP_PRAGMA:
		m_method = std::make_unique<CompilerDialect>();
		break;
	case TK_PP_LINE:
		m_method = MakeMethod<FixLocation>();
		break;
	case TK_PP_WARNING:
		m_method = MakeMethod<LinguisticError>(false);
		break;
	case TK_PP_ERROR:
		m_method = MakeMethod<LinguisticError>();
		break;
	default:
		throw StageBase::StageException{ Name(), "invalid preprocessing directive" };
	}
}

void Preprocessor::Propagate(bool isDirective, DefaultTokenDataPair& tokenData)
{
	assert(tokenData.HasToken() && tokenData.HasData());

	g_tokenSubscription.CallAnyOf(isDirective, tokenData);
}

void Preprocessor::Dispatch(TokenType token, const DataType data)
{
	// Call the method factory and store the next method as continuation
	if (!m_method) {
		return MethodFactory(token);
	}

	// If continuation is set, continue on
	m_method->Dispence(token, data);
}

void Preprocessor::EndOfLine()
{
	//TODO: call Yield() first
	// Reste directive method for next preprocessor line
	m_method.reset();
}
