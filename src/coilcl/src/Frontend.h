// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

// Local includes.
#include "Profile.h"
#include "Tokenizer.h"

// Project includes.
#include <CryCC/Program.h>

namespace CoilCl
{

// Based on the input and meta provided by the profile, the
// frontend will select the necessary stages. Incompatible options
// should be detected right away and reported back to the user
class Frontend : public CryCC::Program::Stage<Frontend>
{
public:
	Frontend(std::shared_ptr<CoilCl::Profile>&, CryCC::Program::ConditionTracker::Tracker&);

	// Implement interface
	virtual std::string Name() const { return "Frontend"; }

	Frontend& CheckCompatibility();
	TokenizerPtr SelectTokenizer();

private:
	std::shared_ptr<CoilCl::Profile> m_profile;
};

} // namespace CoilCl
