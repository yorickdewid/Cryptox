// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#define DEFAULT_UNIQUE_CTR 100

namespace CryCC
{
namespace AST
{

class UniqueObj
{
public:
	using unique_type = int; //TODO: remove
	using UniqueType = int;

private:
	mutable UniqueType m_id;

public:
	inline UniqueObj()
	{
		m_id = ++s_id;
	}

	// Get the reference to the identifier.
	inline auto& Id() const noexcept { return m_id; }

	//
	// Compare objects.
	//

	bool operator==(const UniqueObj&) const noexcept;
	bool operator!=(const UniqueObj&) const noexcept;
	bool operator< (const UniqueObj&) const noexcept;
	bool operator> (const UniqueObj&) const noexcept;
	bool operator<=(const UniqueObj&) const noexcept;
	bool operator>=(const UniqueObj&) const noexcept;

private:
	static int s_id;
};

} // namespace AST
} // namespace CryCC
