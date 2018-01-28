// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "DirectiveScanner.h"

constexpr char EndOfUnit = '\0';

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

void DirectiveScanner::LexLineDirective()
{
	do {
		Next();
	} while (m_currentChar != '\n' && m_currentChar != '\\' && m_currentChar != EndOfUnit);
}

int DirectiveScanner::Lex()
{
	m_data.reset();
	m_lastTokenLine = m_currentLine;
	while (m_currentChar != EndOfUnit) {
		int token = PreprocessLexSet(m_currentChar);
		token = Lexer::DefaultLexSet(m_currentChar);
		if (token == CONTINUE_NEXT_TOKEN) { continue; }
		return token;
	}

	// Halt if enf of unit is reached
	return TK_HALT;
}

DirectiveScanner::DirectiveScanner(std::shared_ptr<CoilCl::Profile>& profile)
	: Lexer{ profile }
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
	AddKeyword("error", TK_PP_ERROR);
}
