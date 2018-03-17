// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "Sequencer.h"

#include <iostream>

using namespace CoilCl::Emit::Sequencer;

//TODO: const
// AIIPX marker to recognize this particular sequencer
static uint8_t initMarker[] = { 0x9, 0x3, 0xef, 0x17 };

using OutputCallback = std::function<void(uint8_t *data, size_t sz)>;

class ChildGroup : public Serializable::ChildGroupInterface
{
	std::stringstream& m_ss;
	size_t m_elements = 0;
	std::vector<int> m_nodeIdList;

	// Retrieve node ids from stream at the current position
	void RetrieveNodes()
	{
		if (!m_nodeIdList.empty()) { return; }

		uint32_t nodeId;
		for (size_t i = 0; i < m_elements; i++)
		{
			m_ss.read(reinterpret_cast<char *>(&nodeId), sizeof(uint32_t));
			m_nodeIdList.push_back(static_cast<int>(nodeId));
		}
	}

public:
	ChildGroup(std::stringstream& ss, bool read = true)
		: m_ss{ ss }
	{
		// Read the initial data from the input stream
		if (!read) {
			m_ss.read(reinterpret_cast<char *>(&m_elements), sizeof(uint32_t));
			assert(m_elements > 0);
		}
	}

	virtual void SaveNode(std::shared_ptr<ASTNode>& node)
	{
		m_ss.write(reinterpret_cast<const char *>(&node->Id()), sizeof(uint32_t));
		m_nodeIdList.push_back(node->Id());
	}

	virtual int LoadNode(int index)
	{
		RetrieveNodes();

		assert(m_nodeIdList.size() > index);
		return m_nodeIdList[index];
	}

	virtual void SetSize(size_t size)
	{
		// Read the number of groups elements to the stream
		auto _size = static_cast<uint32_t>(size);
		m_ss.write(reinterpret_cast<const char *>(&_size), sizeof(uint32_t));
		m_elements = size;
	}

	virtual size_t GetSize() noexcept
	{
		RetrieveNodes();
		return m_elements;
	}
};

class Visitor : public Serializable::Interface
{
	int level;
	int nodeId;
	int parentId;
	std::stringstream ss;

public:
	Visitor()
		: level{ 0 }
		, nodeId{ 0 }
		, parentId{ 0 }
	{
	}

	Visitor(Visitor& other)
	{
		parentId = other.nodeId;
		level = other.level + 1;
	}

	Visitor(Visitor&&) = delete;

	int Level() { return level; }

	// Create list of child groups and write the number of groups to the 
	// output stream. Each child group in the list is allocated with the 
	// output stream.
	virtual Serializable::GroupListType CreateChildGroups(size_t size)
	{
		// Write the number of groups to the stream
		auto _size = static_cast<uint32_t>(size);
		ss.write(reinterpret_cast<const char *>(&_size), sizeof(uint32_t));

		Serializable::GroupListType group;
		for (size_t i = 0; i < size; i++)
		{
			group.push_back(std::make_shared<ChildGroup>(ss));
		}
		
		return group;
	}

	virtual Serializable::GroupListType GetChildGroups()
	{
		// Write the number of groups to the stream
		uint32_t size = 0;
		ss.read(reinterpret_cast<char *>(&size), sizeof(uint32_t));
		assert(size > 0);

		Serializable::GroupListType group;
		for (size_t i = 0; i < size; i++)
		{
			group.push_back(std::make_shared<ChildGroup>(ss, false));
		}
		
		return group;
	}

	// Set the node id
	virtual void SetId(int id) { nodeId = id; }

	// Stream node data into visitor
	virtual void operator<<(int i) { ss.write(reinterpret_cast<const char *>(&i), sizeof(uint32_t)); }
	virtual void operator<<(double d) { ss << d; }
	virtual void operator<<(bool b) { ss << b; }
	virtual void operator<<(AST::NodeID n) { ss.write(reinterpret_cast<const char *>(&n), sizeof(AST::NodeID)); }
	virtual void operator<<(std::string s) { ss << s; }

	// Stream node data from visitor
	virtual void operator>>(int& i) { ss.read(reinterpret_cast<char *>(&i), sizeof(uint32_t)); }
	virtual void operator>>(double& d) { ss >> d; }
	virtual void operator>>(bool& b) { ss >> b; }
	virtual void operator>>(AST::NodeID& n) { ss.read(reinterpret_cast<char *>(&n), sizeof(AST::NodeID)); }
	virtual void operator>>(std::string& s) { ss >> s; }

	// Write output to streaming backend
	void WriteOutput(std::function<void(uint8_t *data, size_t sz)>& outputCallback)
	{
		outputCallback((uint8_t*)ss.str().c_str(), ss.str().size());
		ss.str(std::string{});
	}

	// Write output to streaming backend
	void WriteOutput(std::function<void(std::vector<uint8_t>&)>& outputCallback)
	{
		std::vector<uint8_t> t;
		outputCallback(t);
	}
};

void CompressNode(ASTNode *node, Visitor visitor, OutputCallback callback)
{
	//std::cout << "visitor.Level " << visitor.Level() << std::endl;

	node->Serialize(visitor);

	// Let the visitor determine how to write the output to the stream
	visitor.WriteOutput(callback);
	for (const auto& weakChild : node->Children()) {
		if (auto delegateChildren = weakChild.lock()) {
			CompressNode(delegateChildren.get(), visitor, callback);
		}
	}
}

void AIIPX::Execute(ASTNode *node)
{
	Visitor visit;
	assert(node);

	m_outputCallback(&initMarker[0], static_cast<size_t>(sizeof(initMarker)));
	CompressNode(node, visit, m_outputCallback);
}
