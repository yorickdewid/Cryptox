// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "Lexer.h"
#include "Preprocessor.h"
#include "IntrusiveScopedPtr.h"

#include <set>

namespace CoilCl
{

// TODO: move into header
enum PreprocessorToken
{
	//TODO: add preprocessor token
	TK_PREPROCESS = 35, // #
	TK_LINE_CONT = 36,  // \.
	TK_LINE_NEW = 37,   // \n

	// Keywords
	TK_PP_INCLUDE = 100, // include
	TK_PP_DEFINE = 101,  // define
	TK_PP_UNDEF = 102,   // undef
	TK_PP_IFDEF = 103,   // ifdef
	TK_PP_IFNDEF = 104,  // ifndef
	TK_PP_ELIF = 105,    // elif
	TK_PP_ENDIF = 106,   // endif
	TK_PP_PRAGMA = 107,  // pragma
	TK_PP_LINE = 108,    // line
	TK_PP_WARNING = 109, // warning
	TK_PP_ERROR = 110,   // error

	// Compiler translation
	TK___LINE__ = 390,
	TK___FILE__ = 391,
};

struct DirectiveKeyword
{
	DirectiveKeyword(PreprocessorToken token)
		: m_token{ token }
	{
	}

	DirectiveKeyword(int token)
		: m_token{ static_cast<PreprocessorToken>(token) }
	{
	}

	// Return token as string
	std::string Print();

private:
	PreprocessorToken m_token;
};

//using DataValuePointer = IntrusiveScopedPtr<Valuedef::Value>;

template<typename _Ty>
class TokenProcessorProxy
{
	_Ty tokenProcessor;

protected:
	std::set<int> m_subscribedTokens;

public:
	TokenProcessorProxy(std::shared_ptr<Profile>&);

	// Connection between scanner and token processor.
	int operator()(std::function<int()>,
				   std::function<bool()>,
				   std::function<Tokenizer::ValuePointer()>,
				   std::function<void(Tokenizer::ValuePointer&)>);

	// Token processor must accede token processor contract.
	static_assert(std::is_base_of<TokenProcessor, _Ty>::value, "");
};

// The directive scanner is an extension on the default lexer
// and adds tokens and opertions to allow macro expansions
class DirectiveScanner : public Lexer
{
	TokenProcessorProxy<Preprocessor> m_proxy;

public:
	DirectiveScanner(std::shared_ptr<Profile>&);

	// Push machine state forward
	virtual int Lex() override;

private:
	int LexWrapper();

	int PreprocessLexSet(char lexChar);
};

} // namespace CoilCl
