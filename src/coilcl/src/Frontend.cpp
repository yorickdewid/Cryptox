// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "Frontend.h"
#include "Lexer.h"
#include "DirectiveScanner.h"

using namespace CoilCl;

Frontend::Frontend(std::shared_ptr<CoilCl::Profile>& profile)
	: Stage{ this }
	, m_profile{ profile }
{
}

Frontend& Frontend::CheckCompatibility()
{
	//TODO
	return (*this);
}

TokenizerPtr Frontend::SelectTokenizer()
{
	/*auto tokenizer = Frontend::Factory(m_profile->Language());
	tokenizer->SetOptions(...);
	return tokenizer;*/
	return std::make_shared<DirectiveScanner>(m_profile);
}
