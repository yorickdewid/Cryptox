// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "Valuedef.h"

#include <functional>

namespace CoilCl
{

// Tokenizer interface used by subsequent stages. Next stags
// and token consumers must interact with the methods directed
// by this interface.
struct Tokenizer
{
	// Handler type for error handling.
	using ErrorHandler = std::function<void(const std::string& msg, char token, int line, int column)>;
	
	// Pointer type to data object.
	using ValuePointer = std::shared_ptr<Valuedef::Value>;

	// Register error handler.
	inline void RegisterErrorHandler(const ErrorHandler errHandler) { errHandlerFunc = errHandler; }

	// Return whether or not the current state yields data.
	virtual bool HasData() const = 0;

	// Retrieve data, data must be freed by caller.
	virtual ValuePointer Data()  = 0;

	// Return if tokenizer is done.
	virtual bool IsDone() const = 0;

	// Current token line.
	virtual int TokenLine() const = 0;

	// Current token column.
	virtual int TokenColumn() const = 0;

	// Move tokenizer into next machine state.
	virtual int Lex() = 0;

protected:
	ErrorHandler errHandlerFunc;
};

using TokenizerPtr = std::shared_ptr<Tokenizer>;

} // namespace CoilCl
