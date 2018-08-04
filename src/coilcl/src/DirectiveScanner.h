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

// TODO: move into header
enum PreprocessorToken
{
	//TODO: add preprocessor token.
	TK_PREPROCESS = 35, // #
	TK_LINE_CONT = 36,  // \.
	TK_LINE_NEW = 37,   // \n

	// Keywords.
	TK_PP_INCLUDE = 100, // include
	TK_PP_DEFINE = 101,  // define
	TK_PP_DEFINED = 102, // defined
	TK_PP_UNDEF = 103,   // undef
	TK_PP_IFDEF = 104,   // ifdef
	TK_PP_IFNDEF = 105,  // ifndef
	TK_PP_ELIF = 106,    // elif
	TK_PP_ENDIF = 107,   // endif
	TK_PP_PRAGMA = 108,  // pragma
	TK_PP_LINE = 109,    // line
	TK_PP_MESSAGE = 110, // message
	TK_PP_WARNING = 111, // warning
	TK_PP_ERROR = 112,   // error

	// Compiler translation.
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

	//TODO: Cry::ToString()
	// Return token as string.
	std::string Print();

private:
	PreprocessorToken m_token;
};

//TODO:
// - Include the proxy in the DirectiveScanner
// - Allow multiple preprocessors
template<typename PreprocessorClass>
class TokenProcessorProxy
{
	class ProfileWrapper;

	std::set<int> m_subscribedTokens;
	std::shared_ptr<Profile> m_profile;
	PreprocessorClass tokenProcessor;

private:
	std::unique_ptr<std::deque<TokenProcessor::TokenDataPair<TokenProcessor::TokenType, const TokenProcessor::DataType>>> m_tokenBacklog;

protected:
	TokenProcessor::TokenDataPair<TokenProcessor::TokenType, const TokenProcessor::DataType> ProcessBacklog();

public:
	template<typename... ArgTypes>
	TokenProcessorProxy(std::shared_ptr<Profile>&, CryCC::Program::ConditionTracker::Tracker&, ArgTypes&&...);

	// Connection between scanner and token processor.
	int operator()(std::function<int()>,
		std::function<bool()>,
		std::function<Tokenizer::ValuePointer()>,
		std::function<void(const Tokenizer::ValuePointer&)>);

	// Token processor must accede token processor contract.
	static_assert(std::is_base_of<TokenProcessor, PreprocessorClass>::value, "");
};

// The directive scanner is an extension on the default lexer
// and adds tokens and opertions to allow macro expansions.
class DirectiveScanner : public Lexer
{
	TokenProcessorProxy<Preprocessor> m_proxy;

public:
	DirectiveScanner(std::shared_ptr<Profile>&, CryCC::Program::ConditionTracker::Tracker&);

	// Push machine state forward.
	virtual int Lex() override;

private:
	int LexWrapper();

	int PreprocessLexSet(char lexChar);
};

} // namespace CoilCl
