// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "coilcl.h" //TODO: should not be a required file

#include <string>
#include <memory>

namespace Compiler //TODO: rename to CoiCl
{

//TODO: add stage option structs

struct Profile
{
	virtual std::string ReadInput() = 0;
	virtual bool Include(const std::string&) = 0;
	virtual std::shared_ptr<metainfo_t> MetaInfo() = 0;
	virtual void Error(const std::string& message, bool isFatal) = 0;
};

} // namespace Compiler
