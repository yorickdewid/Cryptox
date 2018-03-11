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
public:
	virtual void operator<<(int i) { CRY_UNUSED(i); }
	virtual void operator>>(int i) { CRY_UNUSED(i); }
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

	virtual Serializable::GroupListType CreateChildGroups(size_t size)
	{
		return Serializable::GroupListType{ size, std::make_shared<ChildGroup>() };
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
