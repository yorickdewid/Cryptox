// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <CryCC/AST/ASTNode.h>

#include <boost/format.hpp>

namespace CryCC
{
namespace AST
{

//
// Literal.
//

Literal::Literal(const value_type& value)
	: Returnable{ value.Type() }
	, m_value{ value }
{
}

Literal::Literal(value_type&& value)
	: Returnable{ value.Type() }
	, m_value{ std::move(value) }
{
}

Literal::Literal(Serializable::VisitorInterface&)
	: m_value{ Util::MakeInt2(0) } //TODO: temporary fix
{
}

void Literal::Serialize(Serializable::VisitorInterface& pack)
{
	pack << nodeId;

	if (HasReturnType()) {
		pack << true;
		Cry::ByteArray buffer;
		Typedef::TypeFacade::Serialize(ReturnType(), buffer);
		pack << buffer;
	}
	else {
		pack << false;
	}

	ASTNode::Serialize(pack);
}

void Literal::Deserialize(Serializable::VisitorInterface& pack)
{
	NodeID _nodeId;
	pack >> _nodeId;
	AssertNode(_nodeId, nodeId);

	bool hasReturn = false;
	pack >> hasReturn;
	if (hasReturn) {
		Cry::ByteArray buffer;
		pack >> buffer;
		Typedef::TypeFacade::Deserialize(UpdateReturnType(), buffer);
	}

	ASTNode::Deserialize(pack);
}

const std::string Literal::NodeNameImpl(const char *className) const
{
	return boost::str(boost::format("%1$s {%2$d} <line:%3$d,col:%4$d> '%5%' \"%6%\"")
		% RemoveClassFromName(className)
		% m_state.Alteration()
		% m_location.Line() % m_location.Column()
		% ReturnType()->TypeName()
		% m_value.ToString());
}

//
// CharacterLiteral.
//

CharacterLiteral::CharacterLiteral(const Valuedef::Value2& value)
	: Literal{ value }
{
}

CharacterLiteral::CharacterLiteral(Valuedef::Value2&& value)
	: Literal{ std::move(value) }
{
}

CharacterLiteral::CharacterLiteral(char value)
	: Literal{ Util::MakeChar2(value) }
{
}

CharacterLiteral::CharacterLiteral(Serializable::VisitorInterface& pack)
	: Literal{ pack }
{
	Deserialize(pack);
}

void CharacterLiteral::Serialize(Serializable::VisitorInterface& pack)
{
	SerializeImpl<NodeID::CHARACTER_LITERAL_ID>(pack);
}
void CharacterLiteral::Deserialize(Serializable::VisitorInterface& pack)
{
	DeserializeImpl<NodeID::CHARACTER_LITERAL_ID>(pack);
}

const std::string CharacterLiteral::NodeName() const
{
	return Literal::NodeName<CharacterLiteral>();
}

//
// StringLiteral.
//

StringLiteral::StringLiteral(const Valuedef::Value2& value)
	: Literal{ value }
{
}

StringLiteral::StringLiteral(Valuedef::Value2&& value)
	: Literal{ std::move(value) }
{
}

StringLiteral::StringLiteral(const std::string& value)
	: Literal{ Util::MakeString2(value) }
{
}

StringLiteral::StringLiteral(Serializable::VisitorInterface& pack)
	: Literal{ pack }
{
	Deserialize(pack);
}

void StringLiteral::Serialize(Serializable::VisitorInterface& pack)
{
	SerializeImpl<NodeID::STRING_LITERAL_ID>(pack);
}
void StringLiteral::Deserialize(Serializable::VisitorInterface& pack)
{
	DeserializeImpl<NodeID::STRING_LITERAL_ID>(pack);
}

const std::string StringLiteral::NodeName() const
{
	return Literal::NodeName<StringLiteral>();
}

//
// IntegerLiteral.
//

IntegerLiteral::IntegerLiteral(const Valuedef::Value2& value)
	: Literal{ value }
{
}

IntegerLiteral::IntegerLiteral(Valuedef::Value2&& value)
	: Literal{ std::move(value) }
{
}

IntegerLiteral::IntegerLiteral(int value)
	: Literal{ Util::MakeInt2(value) }
{
}

IntegerLiteral::IntegerLiteral(Serializable::VisitorInterface& pack)
	: Literal{ pack }
{
	Deserialize(pack);
}

void IntegerLiteral::Serialize(Serializable::VisitorInterface& pack)
{
	SerializeImpl<NodeID::INTEGER_LITERAL_ID>(pack);
}
void IntegerLiteral::Deserialize(Serializable::VisitorInterface& pack)
{
	DeserializeImpl<NodeID::INTEGER_LITERAL_ID>(pack);
}

const std::string IntegerLiteral::NodeName() const
{
	return Literal::NodeName<IntegerLiteral>();
}

//
// FloatingLiteral.
//

FloatingLiteral::FloatingLiteral(const Valuedef::Value2& value)
	: Literal{ value }
{
}

FloatingLiteral::FloatingLiteral(Valuedef::Value2&& value)
	: Literal{ std::move(value) }
{
}

FloatingLiteral::FloatingLiteral(float value)
	: Literal{ Util::MakeFloat2(value) }
{
}

FloatingLiteral::FloatingLiteral(Serializable::VisitorInterface& pack)
	: Literal{ pack }
{
	Deserialize(pack);
}

void FloatingLiteral::Serialize(Serializable::VisitorInterface& pack)
{
	SerializeImpl<NodeID::FLOAT_LITERAL_ID>(pack);
}
void FloatingLiteral::Deserialize(Serializable::VisitorInterface& pack)
{
	DeserializeImpl<NodeID::FLOAT_LITERAL_ID>(pack);
}

const std::string FloatingLiteral::NodeName() const
{
	return Literal::NodeName<FloatingLiteral>();
}

} // namespace CryCC
} // namespace AST
