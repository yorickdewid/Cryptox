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

template<typename _Ty>
PreprocessorProxy<_Ty>::PreprocessorProxy(std::shared_ptr<Profile>& profile)
	: preprocessor{ profile }
{
}

template<typename _Ty>
int PreprocessorProxy<_Ty>::operator()(std::function<int(void)> backendCall)
{
	int token = -1;
	bool skipNewline = false;
	bool onPreprocLine = false;

	do {
		token = backendCall();
		switch (token) {
		case TK_PREPROCESS:
			onPreprocLine = true;
			skipNewline = false;
			continue;

		case TK_LINE_CONT:
			skipNewline = skipNewline ? false : true;
			continue;

		case TK_LINE_NEW:
			if (!skipNewline && onPreprocLine) {
				onPreprocLine = false;
			}
			continue;
		}

		skipNewline = false;

		// Exit for all non preprocessor tokens
		if (!onPreprocLine) { break;}
		
		//preprocessor.ProcessStatement("kaas");
		puts("fire");
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

int DirectiveScanner::Lex()
{
	return m_proxy([this]()
	{
		return this->LexWrapper();
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
	AddKeyword("if", TK_PP_IF);
	AddKeyword("ifdef", TK_PP_IFDEF);
	AddKeyword("ifndef", TK_PP_IFNDEF);
	AddKeyword("else", TK_PP_ELSE);
	AddKeyword("elif", TK_PP_ELIF);
	AddKeyword("endif", TK_PP_ENDIF);
	AddKeyword("pragma", TK_PP_PRAGMA);
	AddKeyword("line", TK_PP_LINE);
	AddKeyword("error", TK_PP_ERROR);
}
