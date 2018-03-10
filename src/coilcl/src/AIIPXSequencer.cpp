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

//constexpr static const uint8_t initMarker[] = { };
static uint8_t initMarker[] = { 0x9, 0x3, 0xef, 0x17 };

using OutputCallback = std::function<void(uint8_t *data, size_t sz)>;

#if 0
struct UserDataInterface
{
	unsigned int objectId;

	virtual void Serialize() = 0;
	virtual void Deserialize() = 0;
};
#endif

class Visitor : public Serializable::Interface
{
	int level;
	int parentId;
	std::stringstream ss;

public:
	Visitor()
		: level{ 0 }
		, parentId{ 0 }
	{
	}

	Visitor(Visitor& other)
	{
		parentId = other.parentId;
		level = other.level + 1;
	}

	Visitor(Visitor&&) = delete;

	int Level() { return level; }

	virtual void operator<<(int i)
	{
		ss.write(reinterpret_cast<const char *>(&i), sizeof(uint32_t));
	}
	virtual void operator<<(double d)
	{
		ss << d;
	}
	virtual void operator<<(AST::NodeID n)
	{
		ss.write(reinterpret_cast<const char *>(&n), sizeof(AST::NodeID));
	}
	virtual void operator<<(std::string s)
	{
		ss << s;
	}

	virtual void operator>>(int& i)
	{
		ss.read(reinterpret_cast<char *>(&i), sizeof(uint32_t));
	}
	virtual void operator>>(double& d)
	{
		ss >> d;
	}
	virtual void operator>>(AST::NodeID& n)
	{
		ss.read(reinterpret_cast<char *>(&n), sizeof(AST::NodeID));
	}
	virtual void operator>>(std::string& s)
	{
		ss >> s;
	}

	//MarshalUserData(node);

	void WriteOutput(std::function<void(uint8_t *data, size_t sz)>& outputCallback)
	{
		outputCallback((uint8_t*)ss.str().c_str(), ss.str().size());
		ss.str(std::string{});
	}

	void WriteOutput(std::function<void(std::vector<uint8_t>&)>& outputCallback)
	{
		std::vector<uint8_t> t;
		outputCallback(t);
	}
};

template<class _Ty>
struct all_true
{
	using argument_type = _Ty;
	using result_type = bool;

	constexpr bool operator()(const _Ty& _Left) const
	{
		CRY_UNUSED(_Left);
		return true;
	}
};

template<class _Ty>
struct all_false
{
	using argument_type = _Ty;
	using result_type = bool;

	constexpr bool operator()(const _Ty& _Left) const
	{
		CRY_UNUSED(_Left);
		return false;
	}
};

// Use the most complex structure to iterate over a vector
void MarshalUserData(ASTNode *node)
{
	all_true<uintptr_t*> trueComp;
	all_false<uintptr_t*> falseComp;

	auto itStart = node->UserData(trueComp);
	auto itEnd = node->UserData(falseComp);
	for (; itStart != itEnd; ++itStart)
	{
		//TODO: marshall
		std::cout << *(*itStart) << std::endl;
	}
}

void CompressNode(ASTNode *node, Visitor visitor, OutputCallback callback)
{
	//std::cout << "visitor.Level " << visitor.Level() << std::endl;

	node->Serialize(visitor);
	node->Deserialize(visitor);

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
