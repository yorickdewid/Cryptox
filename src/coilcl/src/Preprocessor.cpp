// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be
// copied and/or distributed without the express of the author.

//TODO: Macro expansion

#include "Preprocessor.h"
#include "DirectiveScanner.h"
#include "IntrusiveScopedPtr.h"

#include <stack>
#include <cassert>
#include <iostream>

using namespace CoilCl;

static std::map<std::string, std::string> g_definitionList;
static std::set<std::string> g_sourceGuardList;

namespace Cry
{
namespace Algorithm
{

//template<typename _Ty, typename _KeyTy, class _Predicate>
//void ForEachRangeEqual(_Ty& set, _KeyTy& key, _Predicate p)
//{
//	auto& range = set.equal_range(key);
//	for (auto it = range.first; it != range.second; ++it) {//TODO: still an issue
//		p(it);
//	}
//}

} // namespace Algorithm
} // namespace Cry

static class TokenSubscription
{
public:
	using CallbackFunc = void(*)(Preprocessor::DefaultTokenDataPair&);

public:
	void SubscribeOnToken(int token, CallbackFunc cb)
	{
		m_subscriptionTokenSet.emplace(token, cb);
	}

	// Find token and callback, then erase from set
	void UnsubscribeOnToken(int token, CallbackFunc cb)
	{
		/*Cry::Algorithm::ForEachRangeEqual(m_subscriptionTokenSet, token,
										  [&cb, this](decltype(m_subscriptionTokenSet)::iterator it)
		{
			if (it->second == cb) {
				m_subscriptionTokenSet.erase(it);
			}
		});*/
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
	void CallAnyOf(Preprocessor::DefaultTokenDataPair& tokenData)
	{
		auto range = m_subscriptionTokenSet.equal_range(tokenData.Token());
		for (auto& it = range.first; it != range.second; ++it) {
			(it->second)(tokenData);
		}

		// Invoke any callback function that was registered for all tokens
		std::for_each(m_subscriptionSet.begin(), m_subscriptionSet.end(), [&tokenData](CallbackFunc cb)
		{
			(cb)(tokenData);
		});
	}

private:
	std::multimap<int, CallbackFunc> m_subscriptionTokenSet;
	std::set<CallbackFunc> m_subscriptionSet;
} g_tokenSubscription;

Preprocessor::Preprocessor(std::shared_ptr<CoilCl::Profile>& profile)
	: Stage{ this }
	, m_profile{ profile }
{
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
	virtual void Dispence(int token, const void *data) = 0;

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
	inline _Ty ConvertDataAs(const void *data)
	{
		assert(data);
		return static_cast<const Valuedef::Value*>(data)->As<_Ty>();
	}

	void RequireToken(int expectedToken, int token)
	{
		//TODO:
		if (expectedToken != token) { throw UnexpectedTokenException{ "expected token" }; }
	}

	void RequireData(const void *data)
	{
		// Data was expected, throw if not found
		if (!data) { throw DirectiveException{ "expected constant" }; }
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
	void Dispence(int token, const void *data)
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
			throw DirectiveException{ "include", "expected constant or '<' after 'include'" };
		}
	}
};
#include "Cry/PolyConstructTrait.h"
// Definition and expansion
class DefinitionTag : public AbstractDirective
{
	std::string m_definitionName;
	std::vector<Preprocessor::TokenDataPair<int, const void*>> m_definitionBody;

public:
	void Dispence(int token, const void *data)
	{
		// First item must be the definition name
		if (m_definitionName.empty()) {
			RequireData(data);
			m_definitionName = ConvertDataAs<std::string>(data);
			return;
		}

		//TODO: Add replacement tokens as secondary parameter
		//TODO: Make data intrusive scoped pointer
		auto origValue = static_cast<const Valuedef::Value*>(data);

		//auto val = new Valuedef::ValueObject{ *origVal };
		//auto val = Util::CopyValueObject(origValue);

		//m_definitionBody.push_back(Preprocessor::TokenDataPair<int, const void*>{token, origValue});
	}

	//TODO: replace token, instead of resolv expression, use constant, or whatever floats the boat
	static void OnPropagateCallback(Preprocessor::DefaultTokenDataPair& dataPair)
	{
		using namespace Valuedef;
		using namespace Typedef;

		auto value = static_cast<Valuedef::Value*>(dataPair.Data());
		auto it = g_definitionList.find(value->As<std::string>());
		if (it == g_definitionList.end()) { return; }

		// Create new value and assign new datapointer
		void *newDataObject = new ValueObject<std::string>{ BuiltinType::Specifier::CHAR, it->second };
		dataPair.AssignData(newDataObject);
	}

	~DefinitionTag()
	{
		// Definitions without body are acceptable, in that case push ...
		if (m_definitionBody.empty()) {
			//TODO: Do something ...
		}

		//TODO: Move m_definitionBody into global list
		const auto& result = g_definitionList.insert({ m_definitionName, "kaas" });
		if (!result.second) {
			//TODO: May not be a great move to throw in dtor
			throw DirectiveException{ "define", "'" + m_definitionName + "' already defined" };
		}

		std::cout << "created def " << m_definitionName << std::endl;

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
	void Dispence(int token, const void *data)
	{
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

private:
	int tmpStash;

	// Evaluate expression and return either true for positive
	// result, or false for negative. An evaluation error will
	// throw an exception.
	bool Eval(int expression)
	{
		//TODO: defined(<definition>), defined
		//TODO: &&, ||
		//TODO: >, <, ==, >=, <=, !=, <>, !
		return false;
	}

public:
	void Dispence(int token, const void *data)
	{
		//TODO: Collect all tokens in the stash
	}

	~ConditionalStatement()
	{
		g_tokenSubscription.SubscribeOnAll(&ConditionalStatement::OnPropagateCallback);
		evaluationResult.push(Eval(tmpStash));
	}

	static void OnPropagateCallback(Preprocessor::DefaultTokenDataPair& dataPair)
	{
		// If this token happens to be a conditional statement token, redirect
		switch (dataPair.Token()) {
		case TK_ELSE: { EncounterElse(); return; };
		case TK_PP_ENDIF: { EncounterEndif(); return; }
		}

		if (evaluationResult.empty() || evaluationResult.top()) { return; }

		// Resetting the token indicates the proxy must skip the token
		dataPair.ResetToken();
	}

	static void EncounterElse()
	{
		if (evaluationResult.empty()) {
			throw 1; //TODO: or something else
		}

		// Inverse top most element
		auto& top = evaluationResult.top();
		top = !top;
	}

	static void EncounterEndif()
	{
		if (evaluationResult.empty()) {
			throw 1; //TODO: or something else
		}

		evaluationResult.pop();
		if (evaluationResult.empty()) {
			//TODO: causes SIGSEGV
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
	void Dispence(int token, const void *data)
	{
		RequireToken(TK_IDENTIFIER, token);
		if (HandleTrivialCase(ConvertDataAs<std::string>(data))) { return; }
	}
};

// Set source location to fixed line,col pair
class FixLocation : public AbstractDirective
{
public:
	void Dispence(int token, const void *data)
	{
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

	void Dispence(int token, const void *data)
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

void Preprocessor::MethodFactory(int token)
{
	using namespace ::LocalMethod;

	switch (token) {
	case TK_PP_INCLUDE:
		std::cout << "TK_PP_INCLUDE" << std::endl;
		m_method = MakeMethod<ImportSource>();
		break;
	case TK_PP_DEFINE:
		std::cout << "TK_PP_DEFINE" << std::endl;
		m_method = MakeMethod<DefinitionTag>();
		break;
	case TK_PP_UNDEF:
		std::cout << "TK_PP_UNDEF" << std::endl;
		m_method = MakeMethod<DefinitionUntag>();
		break;
	case TK_IF:
		std::cout << "TK_IF" << std::endl;
		m_method = MakeMethod<ConditionalStatement>();
		break;
	case TK_PP_IFDEF:
		std::cout << "TK_PP_IFDEF" << std::endl;
		m_method = MakeMethod<ConditionalStatement>();
		break;
	case TK_PP_IFNDEF:
		std::cout << "TK_PP_IFNDEF" << std::endl;
		m_method = MakeMethod<ConditionalStatement>();
		break;
	case TK_PP_ELIF:
		std::cout << "TK_PP_ELIF" << std::endl;
		m_method = MakeMethod<ConditionalStatement>();
		break;
	case TK_PP_PRAGMA:
		std::cout << "TK_PP_PRAGMA" << std::endl;
		m_method = std::make_unique<CompilerDialect>();
		break;
	case TK_PP_LINE:
		std::cout << "TK_PP_LINE" << std::endl;
		m_method = MakeMethod<FixLocation>();
		break;
	case TK_PP_WARNING:
		std::cout << "TK_PP_WARNING" << std::endl;
		m_method = MakeMethod<LinguisticError>(false);
		break;
	case TK_PP_ERROR:
		std::cout << "TK_PP_ERROR" << std::endl;
		m_method = MakeMethod<LinguisticError>();
		break;
	default:
		throw StageBase::StageException{ Name(), "invalid preprocessing directive" };
	}
}

void Preprocessor::Propagate(DefaultTokenDataPair& tokenData)
{
	assert(tokenData.HasToken() && tokenData.HasData());

	g_tokenSubscription.CallAnyOf(tokenData);
}

void Preprocessor::Dispatch(int token, const void *data)
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
	// Reste directive method for next preprocessor line
	m_method.reset();
}
