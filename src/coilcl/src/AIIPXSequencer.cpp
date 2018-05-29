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
	std::multimap<int, std::function<void(const std::shared_ptr<AST::ASTNode>&)>> m_nodeHookList;
	std::map<int, std::shared_ptr<AST::ASTNode>> m_passedList;
	InputCallback inputCallback;

	friend ChildGroup;

	void WriteProxy(const std::stringstream::char_type *str, std::streamsize count)
	{
		ss.write(str, count);
	}

	template<typename NativeType>
	void WriteProxy(const NativeType& value);

	void ReadProxy(std::stringstream::char_type *str, std::streamsize count)
	{
		// If stream is empty, redirect read to callback
		if (!ss.rdbuf()->in_avail()) {
			inputCallback(reinterpret_cast<uint8_t *>(str), static_cast<size_t>(count));
			return;
		}

		ss.read(str, count);
	}

	template<typename NativeType>
	void ReadProxy(NativeType& value);

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
	virtual void FireDependencies(std::shared_ptr<AST::ASTNode>&);

	// Stream node data into visitor
	virtual void operator<<(int);
	virtual void operator<<(double);
	virtual void operator<<(bool);
	virtual void operator<<(AST::NodeID);
	virtual void operator<<(std::string);
	virtual void operator<<(std::vector<uint8_t>);

	// Stream node data from visitor
	virtual void operator>>(int&);
	virtual void operator>>(double&);
	virtual void operator>>(bool&);
	virtual void operator>>(AST::NodeID&);
	virtual void operator>>(std::string&);
	virtual void operator>>(std::vector<uint8_t>&);

	// Callback operations
	virtual void operator<<=(std::pair<int, std::function<void(const std::shared_ptr<AST::ASTNode>&)>>);

	// Write output to streaming backend
	void WriteOutput(OutputCallback& outputCallback);
	// Write output to streaming backend
	void WriteOutput(std::function<void(std::vector<uint8_t>&)>& outputCallback);
};

template<>
void Visitor::WriteProxy(const double& value)
{
	WriteProxy(reinterpret_cast<const char *>(&value), sizeof(double));
}

template<>
void Visitor::WriteProxy(const bool& value)
{
	WriteProxy(reinterpret_cast<const char *>(&value), sizeof(bool));
}

template<>
void Visitor::WriteProxy(const std::string& value)
{
	const size_t sz = value.size();
	WriteProxy(reinterpret_cast<const char *>(&sz), sizeof(uint32_t));
	ss << value;
}

template<>
void Visitor::WriteProxy(const std::vector<uint8_t>& value)
{
	const size_t sz = value.size();
	WriteProxy(reinterpret_cast<const char *>(&sz), sizeof(uint32_t));
	WriteProxy(reinterpret_cast<const char *>(value.data()), value.size());
}

template<>
void Visitor::ReadProxy(double& value)
{
	// If stream is empty, redirect read to callback
	if (!ss.rdbuf()->in_avail()) {
		ReadProxy(reinterpret_cast<char *>(&value), sizeof(double));
		return;
	}

	ss >> value;
}

template<>
void Visitor::ReadProxy(bool& value)
{
	// If stream is empty, redirect read to callback
	if (!ss.rdbuf()->in_avail()) {
		ReadProxy(reinterpret_cast<char *>(&value), sizeof(bool));
		return;
	}

	ss >> value;
}

template<>
void Visitor::ReadProxy(std::string& value)
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

	size_t sz = 0;
	ReadProxy(reinterpret_cast<char *>(&sz), sizeof(uint32_t));
	ss >> value;
}

template<>
void Visitor::ReadProxy(std::vector<uint8_t>& value)
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

	size_t sz = 0;
	ReadProxy(reinterpret_cast<char *>(&sz), sizeof(uint32_t));
	value.resize(sz);
	ReadProxy(reinterpret_cast<char *>(value.data()), sz);
}

void Visitor::operator<<(int i) { WriteProxy(reinterpret_cast<const char *>(&i), sizeof(uint32_t)); }
void Visitor::operator<<(double d) { WriteProxy(d); }
void Visitor::operator<<(bool b) { WriteProxy(b); }
void Visitor::operator<<(AST::NodeID n) { WriteProxy(reinterpret_cast<const char *>(&n), sizeof(AST::NodeID)); }
void Visitor::operator<<(std::string s) { WriteProxy(s); }
void Visitor::operator<<(std::vector<uint8_t> b) { WriteProxy(b); }

void Visitor::operator>>(int& i) { ReadProxy(reinterpret_cast<char *>(&i), sizeof(uint32_t)); }
void Visitor::operator>>(double& d) { ReadProxy(d); }
void Visitor::operator>>(bool& b) { ReadProxy(b); }
void Visitor::operator>>(AST::NodeID& n) { ReadProxy(reinterpret_cast<char *>(&n), sizeof(AST::NodeID)); }
void Visitor::operator>>(std::string& s) { ReadProxy(s); }
void Visitor::operator>>(std::vector<uint8_t>& b) { ReadProxy(b); }

class ChildGroup : public Serializable::ChildGroupInterface
{
	Visitor& m_visitor;
	size_t m_elements = 0;
	std::vector<int> m_nodeIdList;

	// Retrieve node ids from stream at the current position
	void RetrieveNodes()
	{
		if (!m_nodeIdList.empty()) { return; }

		// Read the initial data from the input stream if there are no elements
		if (!m_elements) {
			m_visitor.ReadProxy(reinterpret_cast<char *>(&m_elements), sizeof(uint32_t));
			assert(m_elements > 0);
		}

		uint32_t nodeId;
		for (size_t i = 0; i < m_elements; i++)
		{
			m_visitor.ReadProxy(reinterpret_cast<char *>(&nodeId), sizeof(uint32_t));
			m_nodeIdList.push_back(static_cast<int>(nodeId));
		}
	}

public:
	ChildGroup(Visitor *visitor)
		: m_visitor{ (*visitor) }
	{
	}

	virtual void SaveNode(std::shared_ptr<AST::ASTNode>& node)
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
		group.push_back(std::make_shared<ChildGroup>(this));
	}

	return group;
}

void Visitor::FireDependencies(std::shared_ptr<AST::ASTNode>& node)
{
	const auto range = m_nodeHookList.equal_range(node->Id());
	for (auto it = range.first; it != range.second; ++it)
	{
		it->second(node);
	}
	m_nodeHookList.erase(range.first, range.second);
	m_passedList.emplace(node->Id(), node);
}

void Visitor::operator<<=(std::pair<int, std::function<void(const std::shared_ptr<AST::ASTNode>&)>> value)
{
	if (!value.first) { return; }
	auto it = m_passedList.find(value.first);
	if (it != m_passedList.end()) {
		value.second(it->second);
		return;
	}
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

void CompressNode(AST::ASTNode *node, Visitor visitor, OutputCallback callback)
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

AST::AST UncompressNode(Visitor *visitor, InputCallback callback)
{
	using AST::ASTFactory;
	std::shared_ptr<AST::ASTNode> root;

	try
	{
		do {
			// Get node from AST factory
			auto _node = ASTFactory::MakeNode(visitor);
			visitor->Clear();
			assert(_node);

			// Set tree root
			if (!root) {
				root = _node;
			}
		} while (true);
	}
	catch (ASTFactory::EndOfStreamException&)
	{
		// Return root as program tree. The AST wrapper
		// will incorporate the root as tree.
		return root;
	}

	return nullptr;
}

void AIIPX::PackAST(AST::AST tree)
{
	Visitor visit;

	// Write marker to output stream to recognize the sequencer
	m_outputCallback(&initMarker[0], static_cast<size_t>(sizeof(initMarker)));
	CompressNode((*tree), visit, m_outputCallback);
}

void AIIPX::UnpackAST(AST::AST& tree)
{
	// Initialize visitor with input stream
	Visitor visit{ m_inputCallback };

	uint8_t _initMarker[sizeof(initMarker)];
	CRY_MEMZERO(_initMarker, sizeof(initMarker));
	assert(!tree.has_tree());

	// Read marker from input stream
	m_inputCallback(&_initMarker[0], sizeof(initMarker));
	if (memcmp(_initMarker, initMarker, sizeof(initMarker))) {
		throw 1; //TODO
	}

	// Move resulting tree into AST
	tree = std::move(UncompressNode(&visit, m_inputCallback));
}
