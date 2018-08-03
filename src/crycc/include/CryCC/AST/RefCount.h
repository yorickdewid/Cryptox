// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

//TODO: move into Cry::
namespace CryCC
{
namespace AST
{

class RefCount
{
public:
	auto IsUsed() const { return m_useCount > 0; }
	void RegisterCaller() { ++m_useCount; }

protected:
	RefCount() = default;

private:
	int m_useCount = 0;
};

} // namespace AST
} // namespace CryCC

