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
TokenProcessorProxy<_Ty>::TokenProcessorProxy(std::shared_ptr<Profile>& profile)
	: tokenProcessor{ profile }
{
}

template<typename _Ty>
int TokenProcessorProxy<_Ty>::operator()(std::function<int()> lexerLexCall,
										 std::function<bool()> lexerHasDataCall,
										 std::function<Tokenizer::ValuePointer()> lexerDataCall,
										 std::function<void(const Tokenizer::ValuePointer&)> lexerSetDataCall)
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

		// 
		case TK_LINE_CONT:
		{
			skipNewline = skipNewline ? false : true;
			continue;
		}

		//
		case TK_LINE_NEW:
		{
			if (!skipNewline && onPreprocLine) {
				tokenProcessor.EndOfLine();
				onPreprocLine = false;
			}
			continue;
		}
		}

		skipNewline = false;

		// If token contains data, get data pointer.
		Tokenizer::ValuePointer dataPtr = lexerHasDataCall() ? std::move(lexerDataCall()) : nullptr;

		// Before returning back to the frontend caller process present the token and data to the
		// hooked methods. Since token processors can hook onto any token they are allowed
		// to change the token and/or data before continuing downwards. If the hooked methods reset
		// the token, we skip all further operations and continue on with a new token.
		TokenProcessor::DefaultTokenDataPair preprocPair{ token, dataPtr };
		tokenProcessor.Propagate(onPreprocLine, preprocPair);

		// If the token processor cleared the token, we must not return and request
		// next token instead. This allows the token processor to skip over tokens.
		if (!preprocPair.HasToken()) { continue; }

		if (preprocPair.HasTokenChanged()) {
			token = preprocPair.Token();
		}

		// If the token contains data, and the data pointer was changed, swap data.
		if (preprocPair.HasData() && preprocPair.HasDataChanged()) {
			lexerSetDataCall(preprocPair.Data());
		}

		// Break for all non token processor items and non subscribed tokens.
		Cry::Algorithm::MatchOn<decltype(token)> pred{ token };
		if (!onPreprocLine && (m_subscribedTokens.empty() || !std::any_of(m_subscribedTokens.cbegin(), m_subscribedTokens.cend(), pred))) {
			break;
		}

		// Call token processor if any of the token conditions was met.
		tokenProcessor.Dispatch(token, dataPtr);
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

	// Halt if end of unit is reached.
	return TK_HALT;
}

int DirectiveScanner::Lex()
{
	// Setup proxy between directive scanner and token processor.
	return m_proxy([this]() { return this->LexWrapper(); },
				   [this]() { return this->HasData(); },
				   [this]() { return m_data; },
				   [this](const Tokenizer::ValuePointer& dataPtr)
	{
		m_data = Tokenizer::ValuePointer{ dataPtr };
	});
}

DirectiveScanner::DirectiveScanner(std::shared_ptr<Profile>& profile)
	: Lexer{ profile }
	, m_proxy{ profile }
{
	AddKeyword("include", TK_PP_INCLUDE);
	AddKeyword("include", TK_PP_INCLUDE);
	AddKeyword("define", TK_PP_DEFINE);
	AddKeyword("undef", TK_PP_UNDEF);
	AddKeyword("ifdef", TK_PP_IFDEF);
	AddKeyword("ifndef", TK_PP_IFNDEF);
	AddKeyword("elif", TK_PP_ELIF);
	AddKeyword("endif", TK_PP_ENDIF);
	AddKeyword("pragma", TK_PP_PRAGMA);
	AddKeyword("line", TK_PP_LINE);
	AddKeyword("warning", TK_PP_WARNING);
	AddKeyword("error", TK_PP_ERROR);
}
