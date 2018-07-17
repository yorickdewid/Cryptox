// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "ASTNode.h"

#include <boost/format.hpp>

ResolveRefExpr::ResolveRefExpr(const std::string& identifier)
	: m_identifier{ identifier }
{
}

void ResolveRefExpr::Serialize(Serializable::Interface& pack)
{
	pack << nodeId;
	pack << m_identifier;
	Expr::Serialize(pack);
}

void ResolveRefExpr::Deserialize(Serializable::Interface& pack)
{
	AST::NodeID _nodeId;
	pack >> _nodeId;
	AssertNode(_nodeId, nodeId);

	pack >> m_identifier;

	Expr::Deserialize(pack);
}

const std::string ResolveRefExpr::NodeName() const
{
	return boost::str(boost::format("%1$s {%2$d} <line:%3$d,col:%4$d> '%5%'")
		% RemoveClassFromName(typeid(ResolveRefExpr).name())
		% m_state.Alteration()
		% line % col
		% m_identifier);
}


void DeclRefExpr::Resolve(const std::shared_ptr<ASTNode>& ref)
{
	m_ref = std::dynamic_pointer_cast<Decl>(ref);
}

bool DeclRefExpr::HasReturnType() const
{
	return IsResolved();
}

void DeclRefExpr::SetReturnType(Typedef::TypeFacade type)
{
	assert(IsResolved());
	return Reference()->SetReturnType(type);
}

const Typedef::TypeFacade& DeclRefExpr::ReturnType() const
{
	assert(IsResolved());
	return Reference()->ReturnType();
}

Typedef::TypeFacade& DeclRefExpr::UpdateReturnType()
{
	assert(IsResolved());
	return Reference()->UpdateReturnType();
}

void DeclRefExpr::Serialize(Serializable::Interface& pack)
{
	pack << nodeId;

	auto group = pack.ChildGroups(1);
	group.Size(1);
	group << m_ref;

	ResolveRefExpr::Serialize(pack);
}

void DeclRefExpr::Deserialize(Serializable::Interface& pack)
{
	AST::NodeID _nodeId;
	pack >> _nodeId;
	AssertNode(_nodeId, nodeId);

	auto group = pack.ChildGroups();
	pack <<= {group[0], [=](const std::shared_ptr<ASTNode>& node) {
		Resolve(node);
	}};

	ResolveRefExpr::Deserialize(pack);
}

const std::string DeclRefExpr::NodeName() const
{
	if (IsResolved()) {
		std::string _node{ RemoveClassFromName(typeid(DeclRefExpr).name()) };
		_node += " {" + std::to_string(m_state.Alteration()) + "}";
		_node += " <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> ";
		_node += "linked '" + m_ref.lock()->Identifier() + "' ";

		if (Reference()->ReturnType().HasValue()) {
			_node += "'" + Reference()->ReturnType().TypeName() + "' ";
			_node += Reference()->ReturnType()->StorageClassName();
		}

		return _node;
	}

	return ResolveRefExpr::NodeName();
}


void CallExpr::Serialize(Serializable::Interface& pack)
{
	pack << nodeId;

	auto group = pack.ChildGroups(2);
	group.Size(1);
	group << NODE_UPCAST(m_funcRef);

	group++;
	group.Size(1);
	group << NODE_UPCAST(m_args);

	Expr::Serialize(pack);
}

void CallExpr::Deserialize(Serializable::Interface& pack)
{
	AST::NodeID _nodeId;
	pack >> _nodeId;
	AssertNode(_nodeId, nodeId);

	auto group = pack.ChildGroups();
	pack <<= {group[0], [=](const std::shared_ptr<ASTNode>& node) {
		ASTNode::AppendChild(node);
		auto ref = std::dynamic_pointer_cast<DeclRefExpr>(node);
		m_funcRef = ref;
	}};

	group++;
	pack <<= {group[0], [=](const std::shared_ptr<ASTNode>& node) {
		ASTNode::AppendChild(node);
		auto args = std::dynamic_pointer_cast<ArgumentStmt>(node);
		m_args = args;
	}};

	Expr::Deserialize(pack);
}

const std::string CallExpr::NodeName() const
{
	std::string _node{ RemoveClassFromName(typeid(CallExpr).name()) };
	_node += " {" + std::to_string(m_state.Alteration()) + "}";
	_node += " <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> ";

	if (ReturnType().HasValue()) {
		_node += "'" + ReturnType().TypeName() + "' ";
		_node += ReturnType()->StorageClassName();
	}

	return _node;
}


BuiltinExpr::BuiltinExpr(std::shared_ptr<DeclRefExpr>& func, std::shared_ptr<DeclRefExpr> expr, std::shared_ptr<ArgumentStmt> args)
	: CallExpr{ func, args }
	, m_expr{ expr }
{
	if (expr != nullptr) {
		ASTNode::AppendChild(NODE_UPCAST(expr));
	}
}

void BuiltinExpr::SetExpression(const std::shared_ptr<ASTNode>& node)
{
	ASTNode::AppendChild(node);
	m_expr = node;

	ASTNode::UpdateDelegate();
}

void BuiltinExpr::SetTypename(std::shared_ptr<Typedef::TypedefBase>& type)
{
	m_typenameType = type;
}

void BuiltinExpr::Serialize(Serializable::Interface& pack)
{
	pack << nodeId;

	auto group = pack.ChildGroups(1);
	group.Size(1);
	group << m_expr;

	//pack << m_typenameType;//TODO

	Expr::Serialize(pack);
}

void BuiltinExpr::Deserialize(Serializable::Interface& pack)
{
	AST::NodeID _nodeId;
	pack >> _nodeId;
	AssertNode(_nodeId, nodeId);

	//pack >> m_typenameType;//TODO

	auto group = pack.ChildGroups();
	pack <<= {group[0], [=](const std::shared_ptr<ASTNode>& node) {
		SetExpression(node);
	}};

	Expr::Deserialize(pack);
}

const std::string BuiltinExpr::NodeName() const
{
	return boost::str(boost::format("%1$s {%2$d} <line:%3$d,col:%4$d> '%5%'")
		% RemoveClassFromName(typeid(BuiltinExpr).name())
		% m_state.Alteration()
		% line % col);
}


CastExpr::CastExpr(std::shared_ptr<ASTNode>& node, std::shared_ptr<Typedef::TypedefBase> type)
{
	SetReturnType(type);
	ASTNode::AppendChild(node);
	m_body = node;
}

void CastExpr::Serialize(Serializable::Interface& pack)
{
	pack << nodeId;

	auto group = pack.ChildGroups(1);
	group.Size(1);
	group << m_body;

	//pack << m_convOp;//TODO

	Expr::Serialize(pack);
}

void CastExpr::Deserialize(Serializable::Interface& pack)
{
	AST::NodeID _nodeId;
	pack >> _nodeId;
	AssertNode(_nodeId, nodeId);

	//pack >> m_convOp;//TODO

	auto group = pack.ChildGroups();
	pack <<= {group[0], [=](const std::shared_ptr<ASTNode>& node) {
		m_body = node;
		ASTNode::AppendChild(node);
	}};

	Expr::Deserialize(pack);
}

const std::string CastExpr::NodeName() const
{
	//TODO: ugly
	if (!ReturnType().HasValue()) {
		return boost::str(boost::format("%1$s {%2$d} <line:%3$d,col:%4$d>")
			% RemoveClassFromName(typeid(CastExpr).name())
			% m_state.Alteration()
			% line % col);
	}
	else {
		return boost::str(boost::format("%1$s {%2$d} <line:%3$d,col:%4$d> '%5%' %6%")
			% RemoveClassFromName(typeid(CastExpr).name())
			% m_state.Alteration()
			% line % col
			% ReturnType().TypeName()
			% ReturnType()->StorageClassName());
	}
}


ImplicitConvertionExpr::ImplicitConvertionExpr(std::shared_ptr<ASTNode>& node, Conv::Cast::Tag convOp)
	: Convertible{ convOp }
	, m_body{ node }
{
	ASTNode::AppendChild(node);
}

void ImplicitConvertionExpr::Serialize(Serializable::Interface& pack)
{
	pack << nodeId;

	auto group = pack.ChildGroups(1);
	group.Size(1);
	group << m_body;

	//pack << m_convOp;//TODO

	Expr::Serialize(pack);
}

void ImplicitConvertionExpr::Deserialize(Serializable::Interface& pack)
{
	AST::NodeID _nodeId;
	pack >> _nodeId;
	AssertNode(_nodeId, nodeId);

	//pack >> m_convOp;//TODO

	auto group = pack.ChildGroups();
	pack <<= {group[0], [=](const std::shared_ptr<ASTNode>& node) {
		m_body = node;
		ASTNode::AppendChild(node);
	}};

	Expr::Deserialize(pack);
}

const std::string ImplicitConvertionExpr::NodeName() const
{
	assert(ReturnType().HasValue());
	return boost::str(boost::format("%1$s {%2$d} <line:%3$d,col:%4$d> '%5%' %6% [%7%]")
		% RemoveClassFromName(typeid(ImplicitConvertionExpr).name())
		% m_state.Alteration()
		% line % col
		% ReturnType().TypeName()
		% ReturnType()->StorageClassName()
		% Conv::Cast::PrintTag(Converter()));
}


ParenExpr::ParenExpr(std::shared_ptr<ASTNode>& node)
	: m_body{ node }
{
	ASTNode::AppendChild(node);
}

void ParenExpr::Serialize(Serializable::Interface& pack)
{
	pack << nodeId;

	auto group = pack.ChildGroups(1);
	group.Size(1);
	group << m_body;

	Expr::Serialize(pack);
}

void ParenExpr::Deserialize(Serializable::Interface& pack)
{
	AST::NodeID _nodeId;
	pack >> _nodeId;
	AssertNode(_nodeId, nodeId);

	auto group = pack.ChildGroups();
	pack <<= {group[0], [=](const std::shared_ptr<ASTNode>& node) {
		m_body = node;
		ASTNode::AppendChild(node);
	}};

	Expr::Deserialize(pack);
}

const std::string ParenExpr::NodeName() const
{
	std::string _node{ RemoveClassFromName(typeid(ParenExpr).name()) };
	_node += " {" + std::to_string(m_state.Alteration()) + "}";
	_node += " <line:" + std::to_string(line) + ",col:" + std::to_string(col) + "> ";

	if (ReturnType().HasValue()) {
		_node += "'" + ReturnType().TypeName() + "' ";
		_node += ReturnType()->StorageClassName();
	}

	return _node;
}


void InitListExpr::AddListItem(const std::shared_ptr<ASTNode>& node)
{
	ASTNode::AppendChild(node);
	m_children.push_back(node);

	ASTNode::UpdateDelegate();
}

void InitListExpr::Serialize(Serializable::Interface& pack)
{
	pack << nodeId;

	auto group = pack.ChildGroups(1);
	group.Size(m_children.size());
	for (const auto& child : m_children) {
		group << child;
	}

	Expr::Serialize(pack);
}

void InitListExpr::Deserialize(Serializable::Interface& pack)
{
	AST::NodeID _nodeId;
	pack >> _nodeId;
	AssertNode(_nodeId, nodeId);

	auto group = pack.ChildGroups();
	for (size_t i = 0; i < group.Size(); ++i)
	{
		int childNodeId = group[i];
		pack <<= {childNodeId, [=](const std::shared_ptr<ASTNode>& node) {
			AddListItem(node);
		}};
	}

	Expr::Deserialize(pack);
}


CompoundLiteralExpr::CompoundLiteralExpr(std::shared_ptr<InitListExpr>& node)
	: m_body{ node }
{
	ASTNode::AppendChild(NODE_UPCAST(node));
}

void CompoundLiteralExpr::Serialize(Serializable::Interface& pack)
{
	pack << nodeId;

	auto group = pack.ChildGroups(1);
	group.Size(1);
	group << m_body;

	Expr::Serialize(pack);
}

void CompoundLiteralExpr::Deserialize(Serializable::Interface& pack)
{
	AST::NodeID _nodeId;
	pack >> _nodeId;
	AssertNode(_nodeId, nodeId);

	auto group = pack.ChildGroups();
	pack <<= {group[0], [=](const std::shared_ptr<ASTNode>& node) {
		m_body = std::dynamic_pointer_cast<InitListExpr>(node);
		ASTNode::AppendChild(node);
	}};

	Expr::Deserialize(pack);
}


ArraySubscriptExpr::ArraySubscriptExpr(std::shared_ptr<DeclRefExpr>& ref, std::shared_ptr<ASTNode>& expr)
	: m_identifier{ ref }
	, m_offset{ expr }
{
	ASTNode::AppendChild(NODE_UPCAST(ref));
	ASTNode::AppendChild(expr);
}

void ArraySubscriptExpr::Serialize(Serializable::Interface& pack)
{
	pack << nodeId;

	auto group = pack.ChildGroups(2);
	group.Size(1);
	group << m_identifier;

	group++;
	group.Size(1);
	group << m_offset;

	Expr::Serialize(pack);
}

void ArraySubscriptExpr::Deserialize(Serializable::Interface& pack)
{
	AST::NodeID _nodeId;
	pack >> _nodeId;
	AssertNode(_nodeId, nodeId);

	auto group = pack.ChildGroups();
	pack <<= {group[0], [=](const std::shared_ptr<ASTNode>& node) {
		m_identifier = std::dynamic_pointer_cast<DeclRefExpr>(node);
		ASTNode::AppendChild(node);
	}};

	group++;
	pack <<= {group[0], [=](const std::shared_ptr<ASTNode>& node) {
		m_offset = node;
		ASTNode::AppendChild(node);
	}};

	Expr::Deserialize(pack);
}


MemberExpr::MemberExpr(MemberType type, const std::string& name, std::shared_ptr<DeclRefExpr>& node)
	: m_memberType{ type }
	, m_name{ name }
	, m_record{ node }
{
	ASTNode::AppendChild(NODE_UPCAST(node));
}

void MemberExpr::Serialize(Serializable::Interface& pack)
{
	pack << nodeId;
	pack << m_name;
	pack << m_memberType;

	auto group = pack.ChildGroups(1);
	group.Size(1);
	group << m_record;

	Expr::Serialize(pack);
}

void MemberExpr::Deserialize(Serializable::Interface& pack)
{
	AST::NodeID _nodeId;
	pack >> _nodeId;
	AssertNode(_nodeId, nodeId);

	pack >> m_name;

	int memberType;
	pack >> memberType;
	m_memberType = static_cast<MemberType>(memberType);

	auto group = pack.ChildGroups();
	pack <<= {group[0], [=](const std::shared_ptr<ASTNode>& node) {
		m_record = std::dynamic_pointer_cast<DeclRefExpr>(node);
		ASTNode::AppendChild(node);
	}};

	Expr::Deserialize(pack);
}

const std::string MemberExpr::NodeName() const
{
	return boost::str(boost::format("%1$s {%2$d} <line:%3$d,col:%4$d> %5% %6%")
		% RemoveClassFromName(typeid(MemberExpr).name())
		% m_state.Alteration()
		% line % col
		% (m_memberType == MemberType::REFERENCE ? "." : "->")
		% m_name);
}
