// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <string>

struct ReaderInterface
{
	using InputDataType = std::string;

	// Fetch the next input chunk. The size parameter is an indication
	// and can be ignored by the interface implementation. The input
	// is returned as a string. An empty string must be returned when
	// the implementation does not return any new input chunks.
	virtual InputDataType FetchNextChunk(size_t) = 0;

	// Retrieve the current source name. This call is optional and is 
	// allowed to return an empty string.
	virtual std::string FetchMetaInfo() = 0;

	// Switch to another source. The requested source is passed as a
	// string to the implementation. This could be a path or an identifier
	// found in the source. Source switching is implementation defined.
	virtual void SwitchSource(const std::string& source) = 0;
};
