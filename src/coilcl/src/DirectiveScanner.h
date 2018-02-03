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

#include <set>

namespace CoilCl
{

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
	TK_PP_IF = 103,      // if
	TK_PP_IFDEF = 104,   // ifdef
	TK_PP_IFNDEF = 105,  // ifndef
	TK_PP_ELSE = 106,    // else
	TK_PP_ELIF = 107,    // elif
	TK_PP_ENDIF = 108,   // endif
	TK_PP_PRAGMA = 109,  // pragma
	TK_PP_LINE = 110,    // line
	TK_PP_WARNING = 111, // warning
	TK_PP_ERROR = 112,   // error

	// Compiler translation
	TK___LINE__ = 390,
	TK___FILE__ = 391,
};

template<typename _Ty>
class PreprocessorProxy
{
	_Ty preprocessor;

protected:
	std::set<int> m_subscribedTokens;

public:
	PreprocessorProxy(std::shared_ptr<Profile>&);

	// Connection between scanner and preprocessor
	int operator()(std::function<int(void)>,
				   std::function<bool(void)>,
				   std::function<void*(void*)>);

	static_assert(std::is_base_of<Stage<_Ty>, _Ty>::value, "");
	static_assert(std::is_base_of<TokenProcessor, _Ty>::value, "");
};

// The directive scanner is an extension on the default lexer
// and adds tokens and opertions to allow macro expansions
class DirectiveScanner : public Lexer
{
	PreprocessorProxy<Preprocessor> m_proxy;

public:
	DirectiveScanner(std::shared_ptr<Profile>&);

	// Push machine state forward
	virtual int Lex() override;

private:
	int LexWrapper();
	void *DataWrapper(void *data);

	int PreprocessLexSet(char lexChar);
};

} // namespace CoilCl
