// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be
// copied and/or distributed without the express of the author.

#include "DirectiveScanner.h"

constexpr char EndOfUnit = '\0';

using namespace CoilCl;

namespace Cry
{
namespace Algorithm
{

template<typename _Ty, _Ty _Val>
struct MatchStatic
{
	constexpr bool operator()(const _Ty& _Match) const
	{
		return (_Match == _Val);
	}
};

template<typename _Ty>
struct MatchOn
{
	const _Ty& m_val;

	constexpr MatchOn(_Ty val)
		: m_val{ val }
	{
	}

	constexpr bool operator()(const _Ty& val) const
	{
		return (val == m_val);
	}
};

} // namespace Algorithm
} // namespace std

template<typename _Ty>
PreprocessorProxy<_Ty>::PreprocessorProxy(std::shared_ptr<Profile>& profile)
	: preprocessor{ profile }
{
}

template<typename _Ty>
int PreprocessorProxy<_Ty>::operator()(std::function<int(void)> lexerLexCall,
									   std::function<bool(void)> lexerHasDataCall,
									   std::function<void*(void*)> lexerDataCall)
{
	int token = -1;
	bool skipNewline = false;
	bool onPreprocLine = false;

	do {
		token = lexerLexCall();
		switch (token) {
		case TK_PREPROCESS:
		{
			onPreprocLine = true;
			skipNewline = false;
			continue;
		}

		case TK_LINE_CONT:
		{
			skipNewline = skipNewline ? false : true;
			continue;
		}

		case TK_LINE_NEW:
		{
			if (!skipNewline && onPreprocLine) {
				preprocessor.EndOfLine();
				onPreprocLine = false;
			}
			continue;
		}
		}

		skipNewline = false;

		// Break for all non preprocessor and non subscribed tokens. Before returning back to
		// the frontend caller process present the token and data to the preprocessor. Since
		// preprocessors can hook onto any token or contained data they are allowed to change
		// the token and/or data before returning back. If the original data pointer differs
		// from the returning pointer, feed the pointer back. The callback operation will
		// swap the pointer wrapper in-place.
		Cry::Algorithm::MatchOn<decltype(token)> pred{ token };
		if (!onPreprocLine && (m_subscribedTokens.empty() || !std::any_of(m_subscribedTokens.cbegin(), m_subscribedTokens.cend(), pred))) {
			void *data = lexerHasDataCall() ? lexerDataCall(nullptr) : nullptr;
			const void *_origData = data;
			preprocessor.Propagate(token, &data);
			if (data != _origData) {
				lexerDataCall(data);
			}
			break;
		}

		// Call preprocessor if any of the token conditions was met
		preprocessor.Dispatch(token, lexerHasDataCall() ? lexerDataCall(nullptr) : nullptr);
	} while (true);

	return token;
}

int DirectiveScanner::PreprocessLexSet(char lexChar)
{
	switch (lexChar) {
	case '#':
		Next();
		return AssembleToken(TK_PREPROCESS);

	case '\\':
		Next();
		return AssembleToken(TK_LINE_CONT);

	case '\n':
		Next();
		return AssembleToken(TK_LINE_NEW);
	}

	return CONTINUE_NEXT_TOKEN;
}

int DirectiveScanner::LexWrapper()
{
	m_data.reset();
	m_lastTokenLine = m_currentLine;
	while (m_currentChar != EndOfUnit) {
		int token = PreprocessLexSet(m_currentChar);
		if (token == CONTINUE_NEXT_TOKEN) {
			token = Lexer::DefaultLexSet(m_currentChar);
			if (token == CONTINUE_NEXT_TOKEN) { continue; }
		}

		return token;
	}

	// Halt if enf of unit is reached
	return TK_HALT;
}

void *DirectiveScanner::DataWrapper(void *data)
{
	// If data pointer was provided, swap the internal data structure
	if (data) {
		m_data.reset(static_cast<Valuedef::Value*>(data));
	}

	// Steal the poiner from unique pointer
	return static_cast<void *>(m_data.get());
}

int DirectiveScanner::Lex()
{
	return m_proxy([this]() { return this->LexWrapper(); },
				   [this]() { return this->HasData(); },
				   [this](void *data) { return this->DataWrapper(data); });
}

DirectiveScanner::DirectiveScanner(std::shared_ptr<Profile>& profile)
	: Lexer{ profile }
	, m_proxy{ profile }
{
	AddKeyword("include", TK_PP_INCLUDE);
	AddKeyword("include", TK_PP_INCLUDE);
	AddKeyword("define", TK_PP_DEFINE);
	AddKeyword("undef", TK_PP_UNDEF);
	AddKeyword("if", TK_PP_IF);
	AddKeyword("ifdef", TK_PP_IFDEF);
	AddKeyword("ifndef", TK_PP_IFNDEF);
	AddKeyword("else", TK_PP_ELSE);
	AddKeyword("elif", TK_PP_ELIF);
	AddKeyword("endif", TK_PP_ENDIF);
	AddKeyword("pragma", TK_PP_PRAGMA);
	AddKeyword("line", TK_PP_LINE);
	AddKeyword("warning", TK_PP_WARNING);
	AddKeyword("error", TK_PP_ERROR);
}
