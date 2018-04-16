// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "FileReader.h"
#include "StringReader.h"

#include <coilcl.h>

#include <functional>

using BaseReader = std::shared_ptr<Reader>;

struct CompilerAbstraction;

class CompilerContract
{
	// Allow the abstract compiler internal access.
	friend struct CompilerAbstraction;

	// Run compiler.
	virtual program_t Execute() = 0;
	// Set stream chunk size.
	virtual void SetStreamChuckSize(size_t) = 0;
};

struct CompilerAbstraction
{
	CompilerAbstraction(const BaseReader&& reader);
	~CompilerAbstraction();

	// Start backend compiler.
	virtual program_t Start();

	// Set the chunk size as a hint to the reader implementation. This value
	// can be ignored and should bot be relied upon.
	virtual CompilerAbstraction& SetBuffer(size_t);

private:
	CompilerContract * m_compiler = nullptr;
};

// Create a new stream reader object. 
template<typename _Ty, typename... _ArgsTy, typename = typename std::enable_if<std::is_base_of<Reader, _Ty>::value>::type>
inline auto MakeReader(_ArgsTy&&... args) -> BaseReader
{
	return std::make_shared<_Ty>(std::forward<_ArgsTy>(args)...);
}

void GetSectionMemoryBlock(const char *tag, void *program, std::function<void(const char *, size_t)>);
