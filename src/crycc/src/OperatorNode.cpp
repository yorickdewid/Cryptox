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

const char *BinaryOperator::BinOperandStr(BinOperand operand) const
{
	switch (operand) {
	case BinOperand::PLUS:
		return "+";
	case BinOperand::MINUS:
		return "-";
	case BinOperand::MUL:
		return "*";
	case BinOperand::DIV:
		return "/";
	case BinOperand::MOD:
		return "%";
	case BinOperand::ASSGN:
		return "=";
	case BinOperand::XOR:
		return "^";
	case BinOperand::AND:
		return "&";
	case BinOperand::SLEFT:
		return "<<";
	case BinOperand::SRIGHT:
		return ">>";
	case BinOperand::EQ:
		return "==";
	case BinOperand::NEQ:
		return "!=";
	case BinOperand::LT:
		return "<";
	case BinOperand::GT:
		return ">";
	case BinOperand::LE:
		return "<=";
	case BinOperand::GE:
		return ">=";
	case BinOperand::LAND:
		return "&&";
	case BinOperand::LOR:
		return "||";
	}

	return "<unknown>";
}

BinaryOperator::BinaryOperator(BinOperand operand, const std::shared_ptr<ASTNode>& leftSide)
	: m_operand{ operand }
	, m_lhs{ leftSide }
{
	ASTNode::AppendChild(leftSide);
}

void BinaryOperator::SetRightSide(const std::shared_ptr<ASTNode>& node)
{
	ASTNode::AppendChild(node);
	m_rhs = node;

	ASTNode::UpdateDelegate();
}

void BinaryOperator::Emplace(size_t idx, const std::shared_ptr<ASTNode>&& node)
{
	assert(idx == 0 || idx == 1);
	BUMP_STATE();

	ASTNode::RemoveChild(idx);
	ASTNode::AppendChild(node);

	if (idx == 0) {
		m_lhs = std::move(node);
	}
	else {
		m_rhs = std::move(node);
	}

	ASTNode::UpdateDelegate();
}

void BinaryOperator::Serialize(Serializable::VisitorInterface& pack)
{
	pack << nodeId;
	pack << m_operand;

	auto group = pack.ChildGroups(2);
	group.Size(1);
	group << m_lhs;

	group++;
	group.Size(1);
	group << m_rhs;

	Operator::Serialize(pack);
}

void BinaryOperator::Deserialize(Serializable::VisitorInterface& pack)
{
	NodeID _nodeId;
	pack >> _nodeId;
	AssertNode(_nodeId, nodeId);

	int operand;
	pack >> operand;
	m_operand = static_cast<BinOperand>(operand);

	auto group = pack.ChildGroups();
	pack <<= {group[0], [=](const std::shared_ptr<ASTNode>& node) {
		m_lhs = node;
		ASTNode::AppendChild(node);
	}};

	group++;
	pack <<= {group[0], [=](const std::shared_ptr<ASTNode>& node) {
		SetRightSide(node);
	}};

	Operator::Deserialize(pack);
}

const std::string BinaryOperator::NodeName() const
{
	std::string _node{ RemoveClassFromName(typeid(BinaryOperator).name()) };
	_node += " {" + std::to_string(m_state.Alteration()) + "}";
	_node += " <line:" + std::to_string(m_location.Line()) + ",col:" + std::to_string(m_location.Column()) + "> ";

	if (Operator::ReturnType().HasValue()) {
		_node += "'" + Operator::ReturnType().TypeName() + "' ";
		_node += Operator::ReturnType()->StorageClassName();
	}

	_node += "'" + std::string{ BinOperandStr(m_operand) } +"'";

	return _node;
}


ConditionalOperator::ConditionalOperator(std::shared_ptr<ASTNode>& eval, std::shared_ptr<ASTNode> truth, std::shared_ptr<ASTNode> alt)
	: m_evalNode{ eval }
{
	ASTNode::AppendChild(eval);

	if (truth) {
		ASTNode::AppendChild(truth);
		m_truthStmt = truth;
	}

	if (alt) {
		ASTNode::AppendChild(alt);
		m_altStmt = alt;
	}
}

void ConditionalOperator::SetTruthCompound(const std::shared_ptr<ASTNode>& node)
{
	ASTNode::AppendChild(node);
	m_truthStmt = node;

	ASTNode::UpdateDelegate();
}

void ConditionalOperator::SetAltCompound(const std::shared_ptr<ASTNode>& node)
{
	ASTNode::AppendChild(node);
	m_altStmt = node;

	ASTNode::UpdateDelegate();
}

void ConditionalOperator::Serialize(Serializable::VisitorInterface& pack)
{
	pack << nodeId;

	auto group = pack.ChildGroups(3);
	group.Size(1);
	group << m_evalNode;

	group++;
	group.Size(1);
	group << m_truthStmt;

	group++;
	group.Size(1);
	group << m_altStmt;

	Operator::Serialize(pack);
}

void ConditionalOperator::Deserialize(Serializable::VisitorInterface& pack)
{
	NodeID _nodeId;
	pack >> _nodeId;
	AssertNode(_nodeId, nodeId);

	auto group = pack.ChildGroups();
	pack <<= {group[0], [=](const std::shared_ptr<ASTNode>& node) {
		m_evalNode = node;
		ASTNode::AppendChild(node);
	}};

	group++;
	pack <<= {group[0], [=](const std::shared_ptr<ASTNode>& node) {
		SetTruthCompound(node);
	}};

	group++;
	pack <<= {group[0], [=](const std::shared_ptr<ASTNode>& node) {
		SetAltCompound(node);
	}};

	Operator::Deserialize(pack);
}

const std::string ConditionalOperator::NodeName() const
{
	return boost::str(boost::format("%1$s {%2$d} <line:%3$d,col:%4$d>")
		% RemoveClassFromName(typeid(ConditionalOperator).name())
		% m_state.Alteration()
		% m_location.Line() % m_location.Column());
}


const char *UnaryOperator::UnaryOperandStr(UnaryOperand operand) const
{
	switch (operand) {
	case UnaryOperand::INC:
		return "++";
	case UnaryOperand::DEC:
		return "--";
	case UnaryOperand::INTPOS:
		return "+";
	case UnaryOperand::INTNEG:
		return "-";
	case UnaryOperand::ADDR:
		return "&";
	case UnaryOperand::PTRVAL:
		return "*";
	case UnaryOperand::BITNOT:
		return "~";
	case UnaryOperand::BOOLNOT:
		return "!";
	}

	return "<unknown>";
}

UnaryOperator::UnaryOperator(UnaryOperand operand, OperandSide side, const std::shared_ptr<ASTNode>& node)
	: m_operand{ operand }
	, m_side{ side }
{
	ASTNode::AppendChild(node);
	m_body = node;
}

void UnaryOperator::Serialize(Serializable::VisitorInterface& pack)
{
	pack << nodeId;
	pack << m_operand;
	pack << m_side;

	auto group = pack.ChildGroups(3);
	group.Size(1);
	group << m_body;

	Operator::Serialize(pack);
}

void UnaryOperator::Deserialize(Serializable::VisitorInterface& pack)
{
	NodeID _nodeId;
	pack >> _nodeId;
	AssertNode(_nodeId, nodeId);

	int operand;
	pack >> operand;
	m_operand = static_cast<UnaryOperand>(operand);

	int side;
	pack >> side;
	m_side = static_cast<OperandSide>(side);

	auto group = pack.ChildGroups();
	pack <<= {group[0], [=](const std::shared_ptr<ASTNode>& node) {
		m_body = node;
		ASTNode::AppendChild(node);
	}};

	Operator::Deserialize(pack);
}

const std::string UnaryOperator::NodeName() const
{
	return boost::str(boost::format("%1$s {%2$d} <line:%3$d,col:%4$d> %5% '%6%'")
		% RemoveClassFromName(typeid(UnaryOperator).name())
		% m_state.Alteration()
		% m_location.Line() % m_location.Column()
		% (m_side == UnaryOperator::POSTFIX ? "postfix" : "prefix")
		% UnaryOperandStr(m_operand));
}


const char *CompoundAssignOperator::CompoundAssignOperandStr(CompoundAssignOperand operand) const
{
	switch (operand) {
	case CompoundAssignOperand::MUL:
		return "*=";
	case CompoundAssignOperand::DIV:
		return "/=";
	case CompoundAssignOperand::MOD:
		return "%=";
	case CompoundAssignOperand::ADD:
		return "+=";
	case CompoundAssignOperand::SUB:
		return "-=";
	case CompoundAssignOperand::LEFT:
		return "<<=";
	case CompoundAssignOperand::RIGHT:
		return ">>=";
	case CompoundAssignOperand::AND:
		return "&=";
	case CompoundAssignOperand::XOR:
		return "^=";
	case CompoundAssignOperand::OR:
		return "|=";
	}

	return "<unknown>";
}

CompoundAssignOperator::CompoundAssignOperator(CompoundAssignOperand operand, const std::shared_ptr<DeclRefExpr>& node)
	: m_operand{ operand }
{
	ASTNode::AppendChild(NODE_UPCAST(node));
	m_identifier = node;
}

void CompoundAssignOperator::SetRightSide(const std::shared_ptr<ASTNode>& node)
{
	ASTNode::AppendChild(node);
	m_body = node;

	ASTNode::UpdateDelegate();
}

void CompoundAssignOperator::Serialize(Serializable::VisitorInterface& pack)
{
	pack << nodeId;
	pack << m_operand;

	auto group = pack.ChildGroups(3);
	group.Size(1);
	group << m_body;

	group++;
	group.Size(1);
	group << NODE_UPCAST(m_identifier);

	Operator::Serialize(pack);
}

void CompoundAssignOperator::Deserialize(Serializable::VisitorInterface& pack)
{
	NodeID _nodeId;
	pack >> _nodeId;
	AssertNode(_nodeId, nodeId);

	int operand;
	pack >> operand;
	m_operand = static_cast<CompoundAssignOperand>(operand);

	auto group = pack.ChildGroups();
	pack <<= {group[0], [=](const std::shared_ptr<ASTNode>& node) {
		SetRightSide(node);
	}};

	group++;
	pack <<= {group[0], [=](const std::shared_ptr<ASTNode>& node) {
		m_identifier = std::dynamic_pointer_cast<DeclRefExpr>(node);
		ASTNode::AppendChild(node);
	}};

	Operator::Deserialize(pack);
}

const std::string CompoundAssignOperator::NodeName() const
{
	return boost::str(boost::format("%1$s {%2$d} <line:%3$d,col:%4$d> '%5%'")
		% RemoveClassFromName(typeid(CompoundAssignOperator).name())
		% m_state.Alteration()
		% m_location.Line() % m_location.Column()
		% CompoundAssignOperandStr(m_operand));
}

} // namespace CryCC
} // namespace AST
