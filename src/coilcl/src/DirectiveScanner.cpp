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
PreprocessorAdapter<_Ty>::PreprocessorAdapter(std::shared_ptr<Profile>& profile)
	: preprocessor{ profile }
{
}

template<typename _Ty>
int PreprocessorAdapter<_Ty>::BackendInterface(int token)
{
	if (token == TK_PREPROCESS) {

	}
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
	return m_adapter(LexWrapper());
}

DirectiveScanner::DirectiveScanner(std::shared_ptr<Profile>& profile)
	: Lexer{ profile }
	, m_adapter{ profile }
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
