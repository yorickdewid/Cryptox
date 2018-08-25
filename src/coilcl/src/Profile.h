// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <CoilCl/coilcl.h> //TODO: should not be a required file

#include <string>
#include <memory>

namespace CoilCl
{

struct Profile
{
	// Read the input from the frontend, this method will return chunks of data.
	virtual std::string ReadInput() = 0;

	// Ask the frontend to include a source file.
	virtual bool Include(const std::string&) = 0;

	// Request meta information about the current source file.
	virtual std::shared_ptr<metainfo_t> MetaInfo() = 0;

	// Call to report an error to the frontend. 
	virtual void Error(const std::string& message, bool isFatal) = 0;

	// Downcast base to profile interface to limit scope.
	template<typename CompilerBase>
	static auto DeriveInterface(std::shared_ptr<CompilerBase>& compiler)
	{
		static_assert(std::is_base_of<Profile, CompilerBase>::value, "Base must implement Profile");
		return std::dynamic_pointer_cast<Profile>(compiler);
	}
};

} // namespace CoilCl
