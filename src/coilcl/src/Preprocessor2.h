// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "Profile.h"
#include "Stage.h"
#include "Tokenizer.h"

namespace CoilCl
{

class Preprocessor2 : public Stage<Preprocessor2>
{
public:
	Preprocessor2(std::shared_ptr<CoilCl::Profile>&);

	virtual std::string Name() const { return "Preprocessor2"; }

	Preprocessor2& CheckCompatibility();
	Preprocessor2& Process();
	TokenizerPtr DumpTokenizer();

	//private:
	//	void ImportSource(std::string);

private:
	std::shared_ptr<CoilCl::Profile> m_profile;
};

} // namespace CoilCl
