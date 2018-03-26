// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "Sequencer.h"
#include "ASTFactory.h"

#include <iostream>
#include <map>

using namespace CoilCl::Emit::Sequencer;

//TODO: const
// AIIPX marker to recognize this particular sequencer
static uint8_t initMarker[] = { 0x9, 0x3, 0xef, 0x17 };

using OutputCallback = std::function<void(uint8_t *data, size_t sz)>;
using InputCallback = std::function<void(uint8_t *data, size_t sz)>;

class ChildGroup;

class Visitor : public Serializable::Interface
{
	int level;
	int nodeId;
	int parentId;
	std::stringstream ss;
	std::multimap<int, std::function<void(const std::shared_ptr<ASTNode>&)>> m_nodeHookList;
	InputCallback inputCallback;

	friend ChildGroup;

	void WriteProxy(const std::stringstream::char_type *str, std::streamsize count)
	{
		ss.write(str, count);
	}

	template<typename _Ty>
	void WriteProxy(const _Ty& value)
	{
		ss << value;
	}

	template<>
	void WriteProxy(const std::string& value)
	{
		size_t sz = value.size();
		WriteProxy(reinterpret_cast<const char *>(&sz), sizeof(uint32_t));
		ss << value;
	}

	void ReadProxy(std::stringstream::char_type *str, std::streamsize count)
	{
		// If stream is empty, redirect read to callback
		if (!ss.rdbuf()->in_avail()) {
			inputCallback(reinterpret_cast<uint8_t *>(str), count);
			return;
		}

		ss.read(str, count);
	}

	template<typename _Ty>
	void ReadProxy(_Ty& value)
	{
		// If stream is empty, redirect read to callback
		if (!ss.rdbuf()->in_avail()) {
			//TODO ..
			return;
		}

		ss >> value;
	}

	template<>
	void ReadProxy(std::string& value)
	{
		// If stream is empty, redirect read to callback
		if (!ss.rdbuf()->in_avail()) {
			size_t sz = 0;
			ReadProxy(reinterpret_cast<char *>(&sz), sizeof(uint32_t));
			if (!sz) { return; }
			value.resize(sz);
			inputCallback(reinterpret_cast<uint8_t *>(&value[0]), sz);
			return;
		}

		ss >> value;
	}

public:
	// Default constructor
	Visitor();

	// Initialize with input callback
	Visitor(InputCallback& _inputCallback);

	// Copy and increment level
	Visitor(Visitor& other);

	// Prohibit indirect moves
	Visitor(Visitor&&) = delete;

	int Level() { return level; }
	// Clear internal buffer
	void Clear() noexcept { ss = std::stringstream{}; }

	// Create list of child groups and write the number of groups to the 
	// output stream. Each child group in the list is allocated with the 
	// output stream.
	virtual Serializable::GroupListType CreateChildGroups(size_t size);
	//....
	virtual Serializable::GroupListType GetChildGroups();

	// Set the node id
	virtual void SetId(int id) { nodeId = id; }
	// Invoke registered callbacks
	virtual void FireDependencies(std::shared_ptr<ASTNode>&);

	// Stream node data into visitor
	virtual void operator<<(int i) { WriteProxy(reinterpret_cast<const char *>(&i), sizeof(uint32_t)); }
	virtual void operator<<(double d) { WriteProxy(d); }
	virtual void operator<<(bool b) { WriteProxy(b); }
	virtual void operator<<(AST::NodeID n) { WriteProxy(reinterpret_cast<const char *>(&n), sizeof(AST::NodeID)); }
	virtual void operator<<(std::string s) { WriteProxy(s); }

	// Stream node data from visitor
	virtual void operator>>(int& i) { ReadProxy(reinterpret_cast<char *>(&i), sizeof(uint32_t)); }
	virtual void operator>>(double& d) { ReadProxy(d); }
	virtual void operator>>(bool& b) { ReadProxy(b); }
	virtual void operator>>(AST::NodeID& n) { ReadProxy(reinterpret_cast<char *>(&n), sizeof(AST::NodeID)); }
	virtual void operator>>(std::string& s) { ReadProxy(s); }

	// Callback operations
	virtual void operator<<=(std::pair<int, std::function<void(const std::shared_ptr<ASTNode>&)>>);

	// Write output to streaming backend
	void WriteOutput(OutputCallback& outputCallback);
	// Write output to streaming backend
	void WriteOutput(std::function<void(std::vector<uint8_t>&)>& outputCallback);
};

class ChildGroup : public Serializable::ChildGroupInterface
{
	Visitor& m_visitor;
	size_t m_elements = 0;
	std::vector<int> m_nodeIdList;

	// Retrieve node ids from stream at the current position
	void RetrieveNodes()
	{
		if (!m_nodeIdList.empty()) { return; }

		uint32_t nodeId;
		for (size_t i = 0; i < m_elements; i++)
		{
			m_visitor.ReadProxy(reinterpret_cast<char *>(&nodeId), sizeof(uint32_t));
			m_nodeIdList.push_back(static_cast<int>(nodeId));
		}
	}

public:
	ChildGroup(Visitor *visitor, bool read = true)
		: m_visitor{ (*visitor) }
	{
		// Read the initial data from the input stream
		if (!read) {
			m_visitor.ReadProxy(reinterpret_cast<char *>(&m_elements), sizeof(uint32_t));
			assert(m_elements > 0);
		}
	}

	virtual void SaveNode(std::shared_ptr<ASTNode>& node)
	{
		m_visitor.WriteProxy(reinterpret_cast<const char *>(&node->Id()), sizeof(uint32_t));
		m_nodeIdList.push_back(node->Id());
	}
	
	virtual void SaveNode(nullptr_t)
	{
		constexpr const uint32_t n = 0;
		m_visitor.WriteProxy(reinterpret_cast<const char *>(&n), sizeof(uint32_t));
		m_nodeIdList.push_back(n);
	}

	virtual int LoadNode(int index)
	{
		RetrieveNodes();

		assert(m_nodeIdList.size() > static_cast<size_t>(index));
		return m_nodeIdList[index];
	}

	virtual void SetSize(size_t size)
	{
		// Read the number of groups elements to the stream
		auto _size = static_cast<uint32_t>(size);
		m_visitor.WriteProxy(reinterpret_cast<const char *>(&_size), sizeof(uint32_t));
		m_elements = size;
	}

	virtual size_t GetSize() noexcept
	{
		RetrieveNodes();
		return m_elements;
	}
};

Visitor::Visitor()
	: level{ 0 }
	, nodeId{ 0 }
	, parentId{ 0 }
{
}

Visitor::Visitor(InputCallback& _inputCallback)
	: inputCallback{ _inputCallback }
	, level{ 0 }
	, nodeId{ 0 }
	, parentId{ 0 }
{
}

Visitor::Visitor(Visitor& other)
{
	inputCallback = other.inputCallback;
	parentId = other.nodeId;
	level = other.level + 1;
}

Serializable::GroupListType Visitor::CreateChildGroups(size_t size)
{
	// Write the number of groups to the stream
	auto _size = static_cast<uint32_t>(size);
	WriteProxy(reinterpret_cast<const char *>(&_size), sizeof(uint32_t));

	Serializable::GroupListType group;
	for (size_t i = 0; i < size; i++)
	{
		// Create child group and read initial stream data
		group.push_back(std::make_shared<ChildGroup>(this));
	}

	return group;
}

Serializable::GroupListType Visitor::GetChildGroups()
{
	// Write the number of groups to the stream
	uint32_t size = 0;
	ReadProxy(reinterpret_cast<char *>(&size), sizeof(uint32_t));
	assert(size > 0);

	Serializable::GroupListType group;
	for (size_t i = 0; i < size; i++)
	{
		// Create child group
		group.push_back(std::make_shared<ChildGroup>(this, false));
	}

	return group;
}

void Visitor::FireDependencies(std::shared_ptr<ASTNode>& node)
{
	const auto range = m_nodeHookList.equal_range(node->Id());
	for (auto it = range.first; it != range.second; ++it)
	{
		it->second(node);
	}
}

void Visitor::operator<<=(std::pair<int, std::function<void(const std::shared_ptr<ASTNode>&)>> value)
{
	if (!value.first) { return; }
	m_nodeHookList.emplace(std::move(value));
}

void Visitor::WriteOutput(OutputCallback& outputCallback)
{
	outputCallback((uint8_t*)ss.str().c_str(), ss.str().size());
	ss.str(std::string{});
}

void Visitor::WriteOutput(std::function<void(std::vector<uint8_t>&)>& outputCallback)
{
	std::vector<uint8_t> t;
	outputCallback(t);
}

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

void UncompressNode(ASTNode *node, Visitor *visitor, InputCallback callback)
{
	std::shared_ptr<ASTNode> root;

	try
	{
		do {
			// Get node from AST factory
			auto _node = AST::ASTFactory::MakeNode(visitor);
			visitor->Clear();
			assert(_node);

			// Set tree root
			if (!root) {
				root = _node;
			}
		} while (true);
	}
	catch (int e)
	{
		if (e == 2) { //TODO: for now
			//TODO: Inval node, end here
		}
	}
}

void AIIPX::PackAST(ASTNode *node)
{
	Visitor visit;
	assert(node);

	// Write marker to output stream to recognize the sequencer
	m_outputCallback(&initMarker[0], static_cast<size_t>(sizeof(initMarker)));
	CompressNode(node, visit, m_outputCallback);
}

void AIIPX::UnpackAST(ASTNode *node)
{
	Visitor visit{ m_inputCallback };
	uint8_t _initMarker[sizeof(initMarker)];
	CRY_MEMZERO(_initMarker, sizeof(initMarker));

	// Read marker from input stream
	m_inputCallback(&_initMarker[0], sizeof(initMarker));
	if (memcmp(_initMarker, initMarker, sizeof(initMarker))) {
		throw 1; //TODO
	}

	UncompressNode(node, &visit, m_inputCallback);
}
