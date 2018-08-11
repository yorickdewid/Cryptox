// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <CryCC/AST/ASTNode.h>

namespace CryCC
{
namespace AST
{

void ReturnStmt::SetReturnNode(std::shared_ptr<ASTNode>& node)
{
	ASTNode::AppendChild(node);
	m_returnExpr = node;

	ASTNode::UpdateDelegate();
}

void ReturnStmt::Emplace(size_t idx, const std::shared_ptr<ASTNode>&& node)
{
	BUMP_STATE();

	ASTNode::RemoveChild(idx);
	ASTNode::AppendChild(node);
	m_returnExpr = std::move(node);

	ASTNode::UpdateDelegate();
}

void ReturnStmt::Serialize(Serializable::Interface& pack)
{
	pack << nodeId;

	auto group = pack.ChildGroups(1);
	group.Size(1);
	group << m_returnExpr;

	Stmt::Serialize(pack);
}

void ReturnStmt::Deserialize(Serializable::Interface& pack)
{
	NodeID _nodeId;
	pack >> _nodeId;
	AssertNode(_nodeId, nodeId);

	auto group = pack.ChildGroups();
	pack <<= {group[0], [=](const std::shared_ptr<ASTNode>& node) {
		m_returnExpr = node;
		ASTNode::AppendChild(node);
	}};

	Stmt::Deserialize(pack);
}


IfStmt::IfStmt(std::shared_ptr<ASTNode>& eval, std::shared_ptr<ASTNode> truth, std::shared_ptr<ASTNode> alt)
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

void IfStmt::SetTruthCompound(const std::shared_ptr<ASTNode>& node)
{
	ASTNode::AppendChild(node);
	m_truthStmt = node;

	ASTNode::UpdateDelegate();
}

void IfStmt::SetAltCompound(const std::shared_ptr<ASTNode>& node)
{
	ASTNode::AppendChild(node);
	m_altStmt = node;

	ASTNode::UpdateDelegate();
}

void IfStmt::Serialize(Serializable::Interface& pack)
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

	Stmt::Serialize(pack);
}

void IfStmt::Deserialize(Serializable::Interface& pack)
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

	Stmt::Deserialize(pack);
}

const std::string IfStmt::NodeName() const
{
	std::string _node{ RemoveClassFromName(typeid(IfStmt).name()) };
	_node += " {" + std::to_string(m_state.Alteration()) + "}";
	_node += " <line:" + std::to_string(m_location.Line()) + ",col:" + std::to_string(m_location.Column()) + "> ";

	if (m_truthStmt) {
		_node += "notruth ";
	}

	if (m_altStmt == nullptr) {
		_node += "noalt ";
	}

	return _node;
}


SwitchStmt::SwitchStmt(std::shared_ptr<ASTNode>& eval, std::shared_ptr<ASTNode> body)
	: evalNode{ eval }
{
	ASTNode::AppendChild(eval);

	if (body) {
		ASTNode::AppendChild(body);
		m_body = body;
	}
}

void SwitchStmt::SetBody(const std::shared_ptr<ASTNode>& node)
{
	ASTNode::AppendChild(node);
	m_body = node;

	ASTNode::UpdateDelegate();
}

void SwitchStmt::Serialize(Serializable::Interface& pack)
{
	pack << nodeId;

	auto group = pack.ChildGroups(2);
	group.Size(1);
	group << evalNode;

	group++;
	group.Size(1);
	group << m_body;

	Stmt::Serialize(pack);
}

void SwitchStmt::Deserialize(Serializable::Interface& pack)
{
	NodeID _nodeId;
	pack >> _nodeId;
	AssertNode(_nodeId, nodeId);

	auto group = pack.ChildGroups();
	pack <<= {group[0], [=](const std::shared_ptr<ASTNode>& node) {
		evalNode = node;
		ASTNode::AppendChild(node);
	}};

	group++;
	pack <<= {group[0], [=](const std::shared_ptr<ASTNode>& node) {
		SetBody(node);
	}};

	Stmt::Deserialize(pack);
}


WhileStmt::WhileStmt(std::shared_ptr<ASTNode>& eval, std::shared_ptr<ASTNode> body)
	: evalNode{ eval }
{
	ASTNode::AppendChild(eval);

	if (body) {
		ASTNode::AppendChild(body);
		m_body = body;
	}
}

void WhileStmt::SetBody(const std::shared_ptr<ASTNode>& node)
{
	ASTNode::AppendChild(node);
	m_body = node;

	ASTNode::UpdateDelegate();
}

void WhileStmt::Serialize(Serializable::Interface& pack)
{
	pack << nodeId;

	auto group = pack.ChildGroups(2);
	group.Size(1);
	group << evalNode;

	group++;
	group.Size(1);
	group << m_body;

	Stmt::Serialize(pack);
}

void WhileStmt::Deserialize(Serializable::Interface& pack)
{
	NodeID _nodeId;
	pack >> _nodeId;
	AssertNode(_nodeId, nodeId);

	auto group = pack.ChildGroups();
	pack <<= {group[0], [=](const std::shared_ptr<ASTNode>& node) {
		evalNode = node;
		ASTNode::AppendChild(node);
	}};

	group++;
	pack <<= {group[0], [=](const std::shared_ptr<ASTNode>& node) {
		SetBody(node);
	}};

	Stmt::Deserialize(pack);
}


DoStmt::DoStmt(std::shared_ptr<ASTNode>& body, std::shared_ptr<ASTNode> eval)
	: m_body{ body }
{
	ASTNode::AppendChild(body);

	if (eval) {
		ASTNode::AppendChild(eval);
		m_body = eval;
	}
}

void DoStmt::SetEval(const std::shared_ptr<ASTNode>& node)
{
	ASTNode::AppendChild(node);
	evalNode = node;

	ASTNode::UpdateDelegate();
}

void DoStmt::Serialize(Serializable::Interface& pack)
{
	pack << nodeId;

	auto group = pack.ChildGroups(2);
	group.Size(1);
	group << evalNode;

	group++;
	group.Size(1);
	group << m_body;

	Stmt::Serialize(pack);
}

void DoStmt::Deserialize(Serializable::Interface& pack)
{
	NodeID _nodeId;
	pack >> _nodeId;
	AssertNode(_nodeId, nodeId);

	auto group = pack.ChildGroups();
	pack <<= {group[0], [=](const std::shared_ptr<ASTNode>& node) {
		SetEval(node);
	}};

	group++;
	pack <<= {group[0], [=](const std::shared_ptr<ASTNode>& node) {
		m_body = node;
		ASTNode::AppendChild(node);
	}};

	Stmt::Deserialize(pack);
}


ForStmt::ForStmt(std::shared_ptr<ASTNode>& node1, std::shared_ptr<ASTNode>& node2, std::shared_ptr<ASTNode>& node3)
	: m_node1{ node1 }
	, m_node2{ node2 }
	, m_node3{ node3 }
{
	ASTNode::AppendChild(node1);
	ASTNode::AppendChild(node2);
	ASTNode::AppendChild(node3);
}

void ForStmt::SetBody(const std::shared_ptr<ASTNode>& node)
{
	ASTNode::AppendChild(node);
	m_body = node;

	ASTNode::UpdateDelegate();
}

void ForStmt::Serialize(Serializable::Interface& pack)
{
	pack << nodeId;

	auto group = pack.ChildGroups(4);
	group.Size(1);
	group << m_node1;

	group++;
	group.Size(1);
	group << m_node2;

	group++;
	group.Size(1);
	group << m_node3;

	group++;
	group.Size(1);
	group << m_body;

	Stmt::Serialize(pack);
}

void ForStmt::Deserialize(Serializable::Interface& pack)
{
	NodeID _nodeId;
	pack >> _nodeId;
	AssertNode(_nodeId, nodeId);

	auto group = pack.ChildGroups();
	pack <<= {group[0], [=](const std::shared_ptr<ASTNode>& node) {
		m_node1 = node;
		ASTNode::AppendChild(node);
	}};

	group++;
	pack <<= {group[0], [=](const std::shared_ptr<ASTNode>& node) {
		m_node2 = node;
		ASTNode::AppendChild(node);
	}};

	group++;
	pack <<= {group[0], [=](const std::shared_ptr<ASTNode>& node) {
		m_node3 = node;
		ASTNode::AppendChild(node);
	}};

	group++;
	pack <<= {group[0], [=](const std::shared_ptr<ASTNode>& node) {
		SetBody(node);
	}};

	Stmt::Deserialize(pack);
}


DefaultStmt::DefaultStmt(const std::shared_ptr<ASTNode>& body)
	: m_body{ body }
{
	ASTNode::AppendChild(body);
}

void DefaultStmt::Serialize(Serializable::Interface& pack)
{
	pack << nodeId;

	auto group = pack.ChildGroups(1);
	group.Size(1);
	group << m_body;

	Stmt::Serialize(pack);
}

void DefaultStmt::Deserialize(Serializable::Interface& pack)
{
	NodeID _nodeId;
	pack >> _nodeId;
	AssertNode(_nodeId, nodeId);

	auto group = pack.ChildGroups();
	pack <<= {group[0], [=](const std::shared_ptr<ASTNode>& node) {
		m_body = node;
		ASTNode::AppendChild(node);
	}};

	Stmt::Deserialize(pack);
}


CaseStmt::CaseStmt(std::shared_ptr<ASTNode>& name, std::shared_ptr<ASTNode>& body)
	: m_identifier{ name }
	, m_body{ body }
{
	ASTNode::AppendChild(name);
	ASTNode::AppendChild(body);
}

void CaseStmt::Serialize(Serializable::Interface& pack)
{
	pack << nodeId;

	auto group = pack.ChildGroups(2);
	group.Size(1);
	group << m_identifier;

	group++;
	group.Size(1);
	group << m_body;

	Stmt::Serialize(pack);
}

void CaseStmt::Deserialize(Serializable::Interface& pack)
{
	NodeID _nodeId;
	pack >> _nodeId;
	AssertNode(_nodeId, nodeId);

	auto group = pack.ChildGroups();
	pack <<= {group[0], [=](const std::shared_ptr<ASTNode>& node) {
		m_identifier = node;
		ASTNode::AppendChild(node);
	}};

	group++;
	pack <<= {group[0], [=](const std::shared_ptr<ASTNode>& node) {
		m_body = node;
		ASTNode::AppendChild(node);
	}};

	Stmt::Deserialize(pack);
}


void DeclStmt::AddDeclaration(const std::shared_ptr<VarDecl>& node)
{
	ASTNode::AppendChild(NODE_UPCAST(node));
	m_var.push_back(node);

	ASTNode::UpdateDelegate();
}

void DeclStmt::Serialize(Serializable::Interface& pack)
{
	pack << nodeId;

	auto group = pack.ChildGroups(1);
	group.SetSize(m_var.size());
	for (const auto& child : m_var) {
		group << child;
	}

	Stmt::Serialize(pack);
}

void DeclStmt::Deserialize(Serializable::Interface& pack)
{
	NodeID _nodeId;
	pack >> _nodeId;
	AssertNode(_nodeId, nodeId);

	auto group = pack.ChildGroups();
	for (size_t i = 0; i < group.GetSize(); ++i)
	{
		int childNodeId = group[i];
		pack <<= {childNodeId, [=](const std::shared_ptr<ASTNode>& node) {
			auto decl = std::dynamic_pointer_cast<VarDecl>(node);
			AddDeclaration(decl);
		}};
	}

	Stmt::Deserialize(pack);
}


void ArgumentStmt::AppendArgument(const std::shared_ptr<ASTNode>& node)
{
	ASTNode::AppendChild(node);
	m_arg.push_back(node);

	ASTNode::UpdateDelegate();
}

void ArgumentStmt::Emplace(size_t idx, const std::shared_ptr<ASTNode>&& node)
{
	BUMP_STATE();

	ASTNode::RemoveChild(idx);
	ASTNode::AppendChild(node);
	m_arg[idx] = std::move(node);

	ASTNode::UpdateDelegate();
}

void ArgumentStmt::Serialize(Serializable::Interface& pack)
{
	pack << nodeId;

	auto group = pack.ChildGroups(1);
	group.SetSize(m_arg.size());
	for (const auto& child : m_arg) {
		group << child;
	}

	Stmt::Serialize(pack);
}

void ArgumentStmt::Deserialize(Serializable::Interface& pack)
{
	NodeID _nodeId;
	pack >> _nodeId;
	AssertNode(_nodeId, nodeId);

	auto group = pack.ChildGroups();
	for (size_t i = 0; i < group.GetSize(); ++i)
	{
		int childNodeId = group[i];
		pack <<= {childNodeId, [=](const std::shared_ptr<ASTNode>& node) {
			AppendArgument(node);
		}};
	}

	Stmt::Deserialize(pack);
}


void ParamStmt::AppendParamter(const std::shared_ptr<ASTNode>& node)
{
	ASTNode::AppendChild(node);
	m_param.push_back(node);

	ASTNode::UpdateDelegate();
}

void ParamStmt::Serialize(Serializable::Interface& pack)
{
	pack << nodeId;

	auto group = pack.ChildGroups(1);
	group.SetSize(m_param.size());
	for (const auto& child : m_param) {
		group << child;
	}

	Stmt::Serialize(pack);
}

void ParamStmt::Deserialize(Serializable::Interface& pack)
{
	NodeID _nodeId;
	pack >> _nodeId;
	AssertNode(_nodeId, nodeId);

	auto group = pack.ChildGroups();
	for (size_t i = 0; i < group.GetSize(); ++i) {
		int childNodeId = group[i];
		pack <<= {childNodeId, [=](const std::shared_ptr<ASTNode>& node) {
			AppendParamter(node);
		}};
	}

	Stmt::Deserialize(pack);
}


LabelStmt::LabelStmt(const std::string& name, std::shared_ptr<ASTNode>& node)
	: m_name{ name }
	, m_body{ node }
{
	ASTNode::AppendChild(node);
}

void LabelStmt::Serialize(Serializable::Interface& pack)
{
	pack << nodeId;
	pack << m_name;

	auto group = pack.ChildGroups(1);
	group.Size(1);
	group << m_body;

	Stmt::Serialize(pack);
}

void LabelStmt::Deserialize(Serializable::Interface& pack)
{
	NodeID _nodeId;
	pack >> _nodeId;
	AssertNode(_nodeId, nodeId);

	pack >> m_name;

	auto group = pack.ChildGroups();
	pack <<= {group[0], [=](const std::shared_ptr<ASTNode>& node) {
		m_body = node;
		ASTNode::AppendChild(node);
	}};

	Stmt::Deserialize(pack);
}


GotoStmt::GotoStmt(const std::string& name)
	: m_labelName{ name }
{
}

void GotoStmt::Serialize(Serializable::Interface& pack)
{
	pack << nodeId;
	pack << m_labelName;
	Stmt::Serialize(pack);
}

void GotoStmt::Deserialize(Serializable::Interface& pack)
{
	NodeID _nodeId;
	pack >> _nodeId;
	AssertNode(_nodeId, nodeId);

	pack >> m_labelName;

	Stmt::Deserialize(pack);
}

const std::string GotoStmt::NodeName() const
{
	return std::string{ RemoveClassFromName(typeid(GotoStmt).name()) } +" {" + std::to_string(m_state.Alteration()) + "}" + " <line:" + std::to_string(m_location.Line()) + ",col:" + std::to_string(m_location.Column()) + "> '" + m_labelName + "'";
}


void CompoundStmt::AppendChild(const std::shared_ptr<ASTNode>& node)
{
	ASTNode::AppendChild(node);
	m_children.push_back(node);

	ASTNode::UpdateDelegate();
}

void CompoundStmt::Serialize(Serializable::Interface& pack)
{
	pack << nodeId;

	auto group = pack.ChildGroups(1);
	group.SetSize(m_children.size());
	for (const auto& child : m_children) {
		group << child;
	}

	Stmt::Serialize(pack);
}

void CompoundStmt::Deserialize(Serializable::Interface& pack)
{
	NodeID _nodeId;
	pack >> _nodeId;
	AssertNode(_nodeId, nodeId);

	auto group = pack.ChildGroups();
	for (size_t i = 0; i < group.GetSize(); ++i)
	{
		int childNodeId = group[i];
		pack <<= {childNodeId, [=](const std::shared_ptr<ASTNode>& node) {
			AppendChild(node);
		}};
	}

	Stmt::Deserialize(pack);
}

} // namespace CryCC
} // namespace AST
