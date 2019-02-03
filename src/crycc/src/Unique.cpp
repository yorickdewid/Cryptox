// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <CryCC/AST/Unique.h>

#include <iostream>

namespace CryCC::AST
{

// Unique object counter initialization.
int UniqueObj::s_id = DEFAULT_UNIQUE_CTR;

bool UniqueObj::operator==(const UniqueObj& other) const noexcept
{
	return m_id == other.m_id;
}
bool UniqueObj::operator!=(const UniqueObj& other) const noexcept
{
	return m_id != other.m_id;
}
bool UniqueObj::operator<(const UniqueObj& other) const noexcept
{
	return m_id < other.m_id;
}
bool UniqueObj::operator>(const UniqueObj& other) const noexcept
{
	return m_id > other.m_id;
}
bool UniqueObj::operator<=(const UniqueObj& other) const noexcept
{
	return m_id <= other.m_id;
}
bool UniqueObj::operator>=(const UniqueObj& other) const noexcept
{
	return m_id >= other.m_id;
}

} // namespace CryCC::AST
