// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <CryCC/AST/NodeId.h>

#include <Cry/Cry.h>

#include <functional>
#include <memory>
#include <vector>

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

	class ChildGroupIterator;

	using GroupListType = std::vector<std::shared_ptr<ChildGroupInterface>>;

	// TODO: rename to VisitorInterface.
	// Visitor interface between the caller and the callee.
	//
	// The interface is used by the AST nodes on serialize and deserialize
	// operations. The interface must be implemented by the caller and is
	// invoked by the callee (node) and includes both serialization and 
	// deserialization methods.
	class Interface
	{
		GroupListType m_childGroups;

		// Create new child group in implementation.
		virtual GroupListType CreateChildGroups(size_t) = 0;
		// Return all child groups from implementation.
		virtual GroupListType GetChildGroups() = 0;

	public:
		using IdType = int;
		using SizeType = size_t;

		// Node id set by the callee. This is for internal bookkeeping and
		// should no be commited to any storage. This method is optional.
		virtual void SetId(IdType) {}
		// Retrieve the first commited node identifier.
		virtual NodeID GetNodeId() = 0;
		// Invoke registered callbacks by the node construction helper.
		virtual void FireDependencies(std::shared_ptr<ASTNode>&) = 0;

		//
		// Stream out operators.
		//

		virtual void operator<<(int) = 0;
		virtual void operator<<(double) = 0;
		virtual void operator<<(bool) = 0;
		virtual void operator<<(NodeID) = 0;
		virtual void operator<<(std::string) = 0;
		virtual void operator<<(std::vector<uint8_t>) = 0;

		//
		// Stream in operators.
		//

		virtual void operator>>(int&) = 0;
		virtual void operator>>(double&) = 0;
		virtual void operator>>(bool&) = 0;
		virtual void operator>>(NodeID&) = 0;
		virtual void operator>>(std::string&) = 0;
		virtual void operator>>(std::vector<uint8_t>&) = 0;

		// The caller must implement the callback functor with the node id. On deserialize this
		// operator is called to register the callback for every node id. Later in the process when
		// the node with the registered node id is created by the factory the corresponding functor must
		// should be invoked with the just-created node as an argument. This structure allows the
		// reconstuction of parent-child relationships.
		virtual void operator<<=(std::pair<IdType, std::function<void(const std::shared_ptr<ASTNode>&)>>) = 0;

		// Create or retrieve child groups. This method is called from the node
		// and the creating and retrieving methods must be implemented by the caller.
		ChildGroupIterator ChildGroups(SizeType size = 0);
	};

	//FUTURE: More methods and operators to communicate data back and forth
	class ChildGroupIterator final
	{
		GroupListType::iterator m_it;
		GroupListType::iterator m_beginIt;

	public:
		using value_type = typename GroupListType::value_type;
		using allocator_type = typename GroupListType::allocator_type;
		using size_type = typename GroupListType::size_type;
		using difference_type = typename GroupListType::size_type;
		using reference = typename GroupListType::reference;
		using const_reference = typename GroupListType::const_reference;
		using pointer = typename GroupListType::pointer;
		using const_pointer = typename GroupListType::const_pointer;
		using iterator = typename GroupListType::iterator;
		using const_iterator = typename GroupListType::const_iterator;
		using reverse_iterator = typename GroupListType::reverse_iterator;
		using const_reverse_iterator = typename GroupListType::const_reverse_iterator;

		// Constructor receives group type iterator
		ChildGroupIterator(GroupListType::iterator);

		// Get group identifier.
		difference_type Id() const;

		template<typename NodeType, typename = typename std::enable_if<std::is_base_of<ASTNode, NodeType>::value>::type>
		void operator<<(std::shared_ptr<NodeType> ptr)
		{
			if (!ptr) {
				(*m_it)->SaveNode(nullptr);
				return;
			}

			auto astNode = std::dynamic_pointer_cast<ASTNode>(ptr);
			(*m_it)->SaveNode(astNode);
		}
		template<typename NodeType, typename = typename std::enable_if<std::is_base_of<ASTNode, NodeType>::value>::type>
		void operator<<(std::weak_ptr<NodeType> ptr)
		{
			if (std::shared_ptr<ASTNode> astNode = ptr.lock()) {
				(*m_it)->SaveNode(astNode);
				return;
			}

			(*m_it)->SaveNode(nullptr);
		}

		// Return node id.
		int operator[](size_type idx)
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
		size_type Size(size_type sz = 0); //TODO: DEPRECATED: FIXME: REMOVE

		// Set the number of elements in this group.
		void SetSize(size_type sz) { (*m_it)->SetSize(sz); }
		// Get the number of elements in this group.
		size_type GetSize() { return (*m_it)->GetSize(); }
	};

	// Serialize interface.
	virtual void Serialize(Interface&) = 0;
	// Deserialize interface.
	virtual void Deserialize(Interface&) = 0;

protected:
	// Test if te node matches the node id. If not throw an exception.
	void AssertNode(const NodeID& got, const NodeID& exp);
};

} // namespace AST
} // namespace CryCC
