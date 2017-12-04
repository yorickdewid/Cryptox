// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

namespace CoilCl
{
namespace AST
{

class RefCount
{
public:
	auto IsUsed() const { return m_useCount > 0; }

protected:
	RefCount() = default;

	void RegisterCaller() { ++m_useCount; }

private:
	int m_useCount = 0;
};

} // namespace AST
} // namespace CoilCl

