// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <CryCC/AST/NodeId.h>

#include <functional>
#include <memory>
#include <vector>
#include <cassert>

namespace CryCC
{
namespace AST
{

class ASTNode;

struct Serializable
{
	struct ChildGroupInterface
	{
		virtual void SaveNode(std::shared_ptr<ASTNode>&) = 0;
		virtual void SaveNode(nullptr_t) = 0;
		virtual int LoadNode(int) = 0;

		// Set size interface.
		virtual void SetSize(size_t sz) = 0;
		// Get size interface.
		virtual size_t GetSize() noexcept = 0;
	};

	class ChildGroupFacade;

	using GroupListType = std::vector<std::shared_ptr<ChildGroupInterface>>;

	class Interface
	{
		GroupListType m_childGroups;

		virtual GroupListType CreateChildGroups(size_t size) = 0;
		virtual GroupListType GetChildGroups() = 0;

	public:
		// Set the node id.
		virtual void SetId(int id) = 0;
		// Invoke registered callbacks.
		virtual void FireDependencies(std::shared_ptr<ASTNode>&) = 0;

		// Stream out operators.
		virtual void operator<<(int) = 0;
		virtual void operator<<(double) = 0;
		virtual void operator<<(bool) = 0;
		virtual void operator<<(NodeID) = 0;
		virtual void operator<<(std::string) = 0;
		virtual void operator<<(std::vector<uint8_t>) = 0;

		// Stream in operators.
		virtual void operator>>(int&) = 0;
		virtual void operator>>(double&) = 0;
		virtual void operator>>(bool&) = 0;
		virtual void operator>>(NodeID&) = 0;
		virtual void operator>>(std::string&) = 0;
		virtual void operator>>(std::vector<uint8_t>&) = 0;

		// Callback operations.
		virtual void operator<<=(std::pair<int, std::function<void(const std::shared_ptr<ASTNode>&)>>) = 0;

		ChildGroupFacade ChildGroups(size_t size = 0)
		{
			if (size > 0) {
				// Create child groups
				m_childGroups = CreateChildGroups(size);
			}
			else {
				// Retrieve child groups
				m_childGroups = GetChildGroups();
			}
			return m_childGroups.begin();
		}
	};

	//FUTURE: More methods and operators to communicate data back and forth
	class ChildGroupFacade final
	{
		GroupListType::iterator m_it;
		GroupListType::iterator m_beginIt;

	public:
		ChildGroupFacade(GroupListType::iterator it)
			: m_it{ it }
			, m_beginIt{ it }
		{
			assert(m_it == m_beginIt);
		}

		// Get group id.
		auto Id() const { return std::distance(m_beginIt, m_it) + 1; }

		template<typename _Ty, typename = typename std::enable_if<std::is_base_of<ASTNode, _Ty>::value>::type>
		void operator<<(std::shared_ptr<_Ty> ptr)
		{
			if (!ptr) {
				(*m_it)->SaveNode(nullptr);
				return;
			}

			auto astNode = std::dynamic_pointer_cast<ASTNode>(ptr);
			(*m_it)->SaveNode(astNode);
		}
		template<typename _Ty, typename = typename std::enable_if<std::is_base_of<ASTNode, _Ty>::value>::type>
		void operator<<(std::weak_ptr<_Ty> ptr)
		{
			if (std::shared_ptr<ASTNode> astNode = ptr.lock()) {
				(*m_it)->SaveNode(astNode);
				return;
			}

			(*m_it)->SaveNode(nullptr);
		}

		// Return node id.
		int operator[](size_t idx)
		{
			return (*m_it)->LoadNode(static_cast<int>(idx));
		}

		// Move iterator forward.
		void operator++() { ++m_it; }
		void operator++(int) { m_it++; }

		// Move iterator backward.
		void operator--() { --m_it; }
		void operator--(int) { m_it--; }

		// Move iterator.
		void Next() { ++m_it; }
		void Previous() { --m_it; }

		// Get or set element size in group.
		size_t Size(size_t sz = 0) //TODO: DEPRECATED: FIXME: REMOVE
		{
			if (sz > 0) {
				(*m_it)->SetSize(sz);
				return sz;
			}

			return (*m_it)->GetSize();
		}

		// Set the number of elements in this group.
		void SetSize(size_t sz) { (*m_it)->SetSize(sz); }
		// Get the number of elements in this group.
		size_t GetSize() { return (*m_it)->GetSize(); }

		std::vector<std::shared_ptr<ASTNode>> Children() { return {}; }
	};

	// Serialize interface.
	virtual void Serialize(Interface&) = 0;
	// Deserialize interface.
	virtual void Deserialize(Interface&) = 0;

protected:
	void AssertNode(const NodeID& got, const NodeID& exp)
	{
		if (got != exp) {
			throw 2; //TODO: throw something usefull
		}
	}
};

} // namespace AST
} // namespace CryCC
