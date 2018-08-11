// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be
// copied and/or distributed without the express of the author.

#include <CryCC/AST/ASTNode.h>

#include <cassert>

namespace CryCC
{
namespace AST
{

void Serializable::AssertNode(const NodeID& got, const NodeID& exp)
{
	if (got != exp) {
		CryImplExcept(); //TODO: throw something usefull
	}
}

Serializable::ChildGroupIterator Serializable::Interface::ChildGroups(SizeType size)
{
	// Create child groups.
	if (size > 0) {
		m_childGroups = this->CreateChildGroups(size);
	}
	// Retrieve child groups.
	else {
		m_childGroups = this->GetChildGroups();
	}

	return m_childGroups.begin();
}

Serializable::ChildGroupIterator::ChildGroupIterator(GroupListType::iterator it)
	: m_it{ it }
	, m_beginIt{ it }
{
	assert(m_it == m_beginIt);
}

Serializable::ChildGroupIterator::difference_type Serializable::ChildGroupIterator::Id() const
{
	return std::distance(m_beginIt, m_it) + 1;
}

// Get or set element size in group.
Serializable::ChildGroupIterator::size_type Serializable::ChildGroupIterator::Size(size_type sz)
{
	if (sz > 0) {
		(*m_it)->SetSize(sz);
		return sz;
	}

	return (*m_it)->GetSize();
}

} // namespace AST
} // namespace CryCC
