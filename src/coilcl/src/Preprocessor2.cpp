// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <boost/algorithm/string.hpp>

#include <iostream>

#include "Preprocessor2.h"
#include "Lexer.h"

#define PREPROC_TOKEN '#'

#define BITSET(b) (m_bitset & b)

using namespace CoilCl;

Preprocessor2::Preprocessor2(std::shared_ptr<CoilCl::Profile>& profile)
	: Stage{ this }
	, m_profile{ profile }
{
	//
}

Preprocessor2& Preprocessor2::CheckCompatibility()
{
	return (*this);
}

Preprocessor2& Preprocessor2::Process()
{
	return (*this);
	while (true) {
		std::string input = m_profile->ReadInput();
		if (input.empty()) { break; }

		std::cout << input << std::endl;
	}

	return (*this);
}

TokenizerPtr Preprocessor2::DumpTokenizer()
{
	return std::make_shared<Lexer>(m_profile);
}

#if 0

// External sources
void Preprocessor2::ImportSource(std::string source)
{
	source.erase(0, 1);
	source.erase(source.size() - 1);

	//m_profile->Include(source);

	std::cout << "importing '" << source << "'" << std::endl;
}

bool SkipWhitespace(std::string i)
{
	return false;
}



//TODO: Check for __LINE__,__FILE__ last
//TODO: Allow linebreak -> '\'
void Transform(std::string& output)
{
	auto input = m_profile->ReadInput();
	if (input.empty()) {
		return;
	}

	int startlineOffset = 0;

	auto dirty = false;
	for (size_t i = 0; i < input.size(); ++i) {
		if (input[i] == PREPROC_TOKEN && !dirty) {
			const auto endoffset = input.substr(i).find_first_of("\r\n");
			const auto startoffset = ++i;
			const auto _i = startoffset;

			for (; SkipWhitespace(input[i]); ++i);
			const auto& args = input.substr(i, endoffset - (i - _i));
			const auto& op = ProcessStatement(args);
#if 0
			if (op->TokenResult == StatementOperation::TokenDesignator::TOKEN_ERASE) {
				input.erase(startoffset - 1, endoffset);
			}
			else {
				//TODO
				//i += (endoffset - (i - _i));
			}
#endif
		}

		// Found a newline
		if (input[i] == '\n') {
			dirty = false;
			const auto& line = input.substr(startlineOffset, (i + 1) - startlineOffset);
			std::cout << "KAAS: " << line << std::endl;

			//TrapSubscription(StatementOperation::Subscription::ON_EVERY_LINE);

			//TODO: trap line subscriptions
			startlineOffset = i + 1;
			continue;
		}
		// Skip all whitespace
		else if (SkipWhitespace(input[i])) {
			continue;
		}

		//TODO: trap token
		dirty = true;
	}

	// Write altered source to supplied output
	output = input;
}

#endif
