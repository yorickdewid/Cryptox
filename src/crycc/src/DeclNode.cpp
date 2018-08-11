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

void Decl::Serialize(Serializable::Interface& pack)
{
	pack << nodeId;
	pack << m_identifier;

	if (HasReturnType()) {
		pack << true;
		Cry::ByteArray buffer;
		Typedef::TypeFacade::Serialize(int{}, ReturnType(), buffer);
		pack << buffer;
	}
	else {
		pack << false;
	}

	ASTNode::Serialize(pack);
}

void Decl::Deserialize(Serializable::Interface& pack)
{
	NodeID _nodeId;
	pack >> _nodeId;
	AssertNode(_nodeId, nodeId);

	pack >> m_identifier;

	bool hasReturn = false;
	pack >> hasReturn;
	if (hasReturn) {
		Cry::ByteArray buffer;
		pack >> buffer;
		Typedef::TypeFacade::Deserialize(int{}, UpdateReturnType(), buffer);
	}

	ASTNode::Deserialize(pack);
}


VarDecl::VarDecl(const std::string& name, std::shared_ptr<Typedef::TypedefBase> type, std::shared_ptr<ASTNode> node)
	: Decl{ name, type }
	, m_body{ node }
{
	ASTNode::AppendChild(node);
}

void VarDecl::Emplace(size_t idx, const std::shared_ptr<ASTNode>&& node)
{
	BUMP_STATE();

	ASTNode::RemoveChild(idx);
	ASTNode::AppendChild(node);
	m_body = std::move(node);

	ASTNode::UpdateDelegate();
}

void VarDecl::Serialize(Serializable::Interface& pack)
{
	pack << nodeId;

	auto group = pack.ChildGroups(1);
	group.Size(1);
	group << m_body;

	Decl::Serialize(pack);
}

void VarDecl::Deserialize(Serializable::Interface& pack)
{
	NodeID _nodeId;
	pack >> _nodeId;
	AssertNode(_nodeId, nodeId);

	auto group = pack.ChildGroups();
	pack <<= {group[0], [=](const std::shared_ptr<ASTNode>& node) {
		m_body = node;
		ASTNode::AppendChild(node);
	}};

	Decl::Deserialize(pack);
}

const std::string VarDecl::NodeName() const
{
	std::string _node{ RemoveClassFromName(typeid(VarDecl).name()) };
	_node += " {" + std::to_string(m_state.Alteration()) + "}";
	_node += " <line:" + std::to_string(m_location.Line()) + ",col:" + std::to_string(m_location.Column()) + "> ";

	if (RefCount::IsUsed()) {
		_node += "used ";
	}

	_node += m_identifier;
	_node += " '" + ReturnType().TypeName() + "' ";
	_node += ReturnType()->StorageClassName();

	return _node;
}


void ParamDecl::Serialize(Serializable::Interface& pack)
{
	pack << nodeId;
	Decl::Serialize(pack);
}

void ParamDecl::Deserialize(Serializable::Interface& pack)
{
	NodeID _nodeId;
	pack >> _nodeId;
	AssertNode(_nodeId, nodeId);

	Decl::Deserialize(pack);
}

const std::string ParamDecl::NodeName() const
{
	return boost::str(boost::format("%1$s {%2$d} <line:%3$d,col:%4$d> %5% '%6%' %7%")
		% RemoveClassFromName(typeid(ParamDecl).name())
		% m_state.Alteration()
		% m_location.Line() % m_location.Column()
		% (m_identifier.empty() ? "abstract" : m_identifier)
		% ReturnType().TypeName()
		% ReturnType()->StorageClassName());
}


void VariadicDecl::Serialize(Serializable::Interface& pack)
{
	pack << nodeId;
	ASTNode::Serialize(pack);
}

void VariadicDecl::Deserialize(Serializable::Interface& pack)
{
	NodeID _nodeId;

	pack >> _nodeId;
	AssertNode(_nodeId, nodeId);

	ASTNode::Deserialize(pack);
}


TypedefDecl::TypedefDecl(const std::string& name, std::shared_ptr<Typedef::TypedefBase> type)
	: Decl{ name, type }
{
}

void TypedefDecl::Serialize(Serializable::Interface& pack)
{
	pack << nodeId;
	ASTNode::Serialize(pack);
}

void TypedefDecl::Deserialize(Serializable::Interface& pack)
{
	NodeID _nodeId;
	pack >> _nodeId;
	AssertNode(_nodeId, nodeId);

	ASTNode::Deserialize(pack);
}

const std::string TypedefDecl::NodeName() const
{
	return boost::str(boost::format("%1$s {%2$d} <line:%3$d,col:%4$d> %5% '%6%' %7%")
		% RemoveClassFromName(typeid(TypedefDecl).name())
		% m_state.Alteration()
		% m_location.Line() % m_location.Column()
		% m_identifier
		% ReturnType().TypeName()
		% ReturnType()->StorageClassName());
}


FieldDecl::FieldDecl(const std::string& name, std::shared_ptr<Typedef::TypedefBase> type)
	: Decl{ name, type }
{
}

void FieldDecl::SetBitField(const std::shared_ptr<IntegerLiteral>& node)
{
	ASTNode::AppendChild(NODE_UPCAST(node));
	m_bits = node;

	ASTNode::UpdateDelegate();
}

void FieldDecl::Serialize(Serializable::Interface& pack)
{
	pack << nodeId;

	auto group = pack.ChildGroups(1);
	group.Size(1);
	group << m_bits;

	Decl::Serialize(pack);
}

void FieldDecl::Deserialize(Serializable::Interface& pack)
{
	NodeID _nodeId;
	pack >> _nodeId;
	AssertNode(_nodeId, nodeId);

	auto group = pack.ChildGroups();
	pack <<= {group[0], [=](const std::shared_ptr<ASTNode>& node) {
		auto bits = std::dynamic_pointer_cast<IntegerLiteral>(node);
		SetBitField(bits);
	}};

	Decl::Deserialize(pack);
}

const std::string FieldDecl::NodeName() const
{
	return boost::str(boost::format("%1$s {%2$d} <line:%3$d,col:%4$d> %5% '%6%' %7%")
		% RemoveClassFromName(typeid(FieldDecl).name())
		% m_state.Alteration()
		% m_location.Line() % m_location.Column()
		% m_identifier
		% ReturnType().TypeName()
		% ReturnType()->StorageClassName());
}


RecordDecl::RecordDecl(const std::string& name)
	: Decl{ name }
{
}

RecordDecl::RecordDecl(RecordType type)
	: m_type{ type }
{
}

bool RecordDecl::IsAnonymous() const
{
	return m_identifier.empty();
}

void RecordDecl::SetName(const std::string& name)
{
	m_identifier = name;
}

void RecordDecl::AddField(std::shared_ptr<FieldDecl>& node)
{
	ASTNode::AppendChild(NODE_UPCAST(node));
	m_fields.push_back(node);

	ASTNode::UpdateDelegate();
}

RecordDecl::RecordType RecordDecl::Type() const
{
	return m_type;
}

void RecordDecl::Serialize(Serializable::Interface& pack)
{
	pack << nodeId;
	pack << m_type;

	auto group = pack.ChildGroups(1);
	group.Size(m_fields.size());
	for (const auto& child : m_fields) {
		group << child;
	}

	Decl::Serialize(pack);
}

void RecordDecl::Deserialize(Serializable::Interface& pack)
{
	NodeID _nodeId;
	pack >> _nodeId;
	AssertNode(_nodeId, nodeId);

	int type;
	pack >> type;
	m_type = static_cast<RecordType>(type);

	auto group = pack.ChildGroups();
	for (size_t i = 0; i < group.Size(); ++i)
	{
		int childNodeId = group[i];
		pack <<= {childNodeId, [=](const std::shared_ptr<ASTNode>& node) {
			auto field = std::dynamic_pointer_cast<FieldDecl>(node);
			AddField(field);
		}};
	}

	Decl::Deserialize(pack);
}

const std::string RecordDecl::NodeName() const
{
	return boost::str(boost::format("%1$s {%2$d} <line:%3$d,col:%4$d> %5% %6%")
		% RemoveClassFromName(typeid(RecordDecl).name())
		% m_state.Alteration()
		% m_location.Line() % m_location.Column()
		% (m_type == RecordType::STRUCT ? "struct" : "union")
		% (IsAnonymous() ? "anonymous" : m_identifier));
}


EnumConstantDecl::EnumConstantDecl(const std::string& name)
	: Decl{ name }
{
}

void EnumConstantDecl::SetAssignment(std::shared_ptr<ASTNode>& node)
{
	ASTNode::AppendChild(node);
	m_body = node;

	ASTNode::UpdateDelegate();
}

void EnumConstantDecl::Serialize(Serializable::Interface& pack)
{
	pack << nodeId;

	auto group = pack.ChildGroups(1);
	group.Size(1);
	group << m_body;

	ASTNode::Serialize(pack);
}

void EnumConstantDecl::Deserialize(Serializable::Interface& pack)
{
	NodeID _nodeId;

	pack >> _nodeId;
	AssertNode(_nodeId, nodeId);

	auto group = pack.ChildGroups();
	pack <<= {group[0], [=](const std::shared_ptr<ASTNode>& node) {
		m_body = node;
		ASTNode::AppendChild(node);
	}};

	ASTNode::Deserialize(pack);
}

const std::string EnumConstantDecl::NodeName() const
{
	std::string _node{ RemoveClassFromName(typeid(EnumConstantDecl).name()) };
	_node += " {" + std::to_string(m_state.Alteration()) + "}";
	_node += " <line:" + std::to_string(m_location.Line()) + ",col:" + std::to_string(m_location.Column()) + "> ";
	_node += m_identifier;

	if (HasReturnType()) {
		_node += " '" + ReturnType().TypeName() + "' ";
		_node += ReturnType()->StorageClassName();
	}

	return _node;
}


auto EnumDecl::IsAnonymous() const
{
	return m_identifier.empty();
}

void EnumDecl::SetName(const std::string& name)
{
	m_identifier = name;
}

void EnumDecl::AddConstant(std::shared_ptr<EnumConstantDecl>& node)
{
	ASTNode::AppendChild(NODE_UPCAST(node));
	m_constants.push_back(node);

	ASTNode::UpdateDelegate();
}

void EnumDecl::Serialize(Serializable::Interface& pack)
{
	pack << nodeId;

	auto group = pack.ChildGroups(1);
	group.Size(m_constants.size());
	for (const auto& child : m_constants) {
		group << child;
	}

	ASTNode::Serialize(pack);
}

void EnumDecl::Deserialize(Serializable::Interface& pack)
{
	NodeID _nodeId;

	pack >> _nodeId;
	AssertNode(_nodeId, nodeId);

	auto group = pack.ChildGroups();
	for (size_t i = 0; i < group.Size(); ++i) {
		int childNodeId = group[i];
		pack <<= {childNodeId, [=](const std::shared_ptr<ASTNode>& node) {
			auto constant = std::dynamic_pointer_cast<EnumConstantDecl>(node);
			AddConstant(constant);
		}};
	}

	ASTNode::Deserialize(pack);
}

const std::string EnumDecl::NodeName() const
{
	return boost::str(boost::format("%1$s {%2$d} <line:%3$d,col:%4$d> %5%")
		% RemoveClassFromName(typeid(EnumDecl).name())
		% m_state.Alteration()
		% m_location.Line() % m_location.Column()
		% (IsAnonymous() ? "anonymous" : m_identifier));
}


FunctionDecl::FunctionDecl(const std::string& name, std::shared_ptr<CompoundStmt>& node)
	: Decl{ name }
	, m_body{ node }
	, m_isPrototype{ false }
{
	ASTNode::AppendChild(NODE_UPCAST(node));
}

FunctionDecl::FunctionDecl(const std::string& name, std::shared_ptr<Typedef::TypedefBase> type)
	: Decl{ name, type }
{
}

void FunctionDecl::SetCompound(const std::shared_ptr<CompoundStmt>& node)
{
	assert(!m_body);

	ASTNode::AppendChild(NODE_UPCAST(node));
	m_body = node;
	m_isPrototype = false;

	ASTNode::UpdateDelegate();
}

void FunctionDecl::SetSignature(std::vector<Typedef::TypeFacade>&& signature)
{
	assert(!signature.empty());

	m_signature = std::move(signature);
}

void FunctionDecl::SetParameterStatement(const std::shared_ptr<ParamStmt>& node)
{
	assert(!m_params);

	ASTNode::AppendChild(NODE_UPCAST(node));
	m_params = node;

	ASTNode::UpdateDelegate();
}

void FunctionDecl::BindPrototype(const std::shared_ptr<FunctionDecl>& node)
{
	assert(!m_isPrototype);
	assert(m_protoRef.expired());

	m_protoRef = node;
}

void FunctionDecl::Serialize(Serializable::Interface& pack)
{
	pack << nodeId;
	pack << m_isPrototype;

	auto group = pack.ChildGroups(3);
	group.Size(1);
	group << NODE_UPCAST(m_params);

	group++;
	group.Size(1);
	group << NODE_UPCAST(m_body);

	group++;
	group.Size(1);
	group << m_protoRef;

	pack << static_cast<int>(m_signature.size());
	for (const auto& signature : m_signature) {
		Cry::ByteArray buffer;
		Typedef::TypeFacade::Serialize(int{}, signature, buffer);
		pack << buffer;
	}

	Decl::Serialize(pack);
}

void FunctionDecl::Deserialize(Serializable::Interface& pack)
{
	NodeID _nodeId;

	pack >> _nodeId;
	AssertNode(_nodeId, nodeId);

	pack >> m_isPrototype;

	auto group = pack.ChildGroups();
	pack <<= {group[0], [=](const std::shared_ptr<ASTNode>& node) {
		auto param = std::dynamic_pointer_cast<ParamStmt>(node);
		SetParameterStatement(param);
	}};

	group++;
	pack <<= {group[0], [=](const std::shared_ptr<ASTNode>& node) {
		auto body = std::dynamic_pointer_cast<CompoundStmt>(node);
		SetCompound(body);
	}};

	group++;
	pack <<= {group[0], [=](const std::shared_ptr<ASTNode>& node) {
		auto ref = std::dynamic_pointer_cast<FunctionDecl>(node);
		BindPrototype(ref);
	}};

	int _signatureSize;
	pack >> _signatureSize;
	for (; _signatureSize > 0; --_signatureSize) {
		Cry::ByteArray buffer;
		pack >> buffer;

		Typedef::TypeFacade type;
		Typedef::TypeFacade::Deserialize(int{}, type, buffer);
		m_signature.push_back(std::move(type));
	}

	Decl::Deserialize(pack);
}

const std::string FunctionDecl::NodeName() const
{
	std::stringstream ss;
	ss << RemoveClassFromName(typeid(FunctionDecl).name());
	ss << " {" + std::to_string(m_state.Alteration()) + "}";
	ss << " <line:" << m_location.Line() << ",col:" << m_location.Column() << "> ";

	if (IsPrototypeDefinition()) {
		ss << "proto ";
	}
	else if (HasPrototypeDefinition()) {
		ss << "linked ";
	}

	if (RefCount::IsUsed()) {
		ss << "used ";
	}

	ss << m_identifier;

	ss << " '" << ReturnType().TypeName() + " (";
	for (auto it = m_signature.begin(); it != m_signature.end(); ++it) {
		ss << it->TypeName();
		if (m_signature.size() > 1 && it != m_signature.end() - 1) {
			ss << ", ";
		}
	}
	ss << ")' ";

	ss << ReturnType()->StorageClassName() << " ";

	if (ReturnType()->IsInline()) {
		ss << "inline";
	}

	return ss.str();
}


void TranslationUnitDecl::AppendChild(const std::shared_ptr<ASTNode>& node)
{
	ASTNode::AppendChild(node);
	m_children.push_back(node);

	ASTNode::UpdateDelegate();
}

void TranslationUnitDecl::Serialize(Serializable::Interface& pack)
{
	pack << nodeId;

	auto group = pack.ChildGroups(1);
	group.Size(m_children.size());
	for (const auto& child : m_children) {
		group << child;
	}

	Decl::Serialize(pack);
}

void TranslationUnitDecl::Deserialize(Serializable::Interface& pack)
{
	NodeID _nodeId;
	pack >> _nodeId;
	AssertNode(_nodeId, nodeId);

	auto group = pack.ChildGroups();
	for (size_t i = 0; i < group.Size(); ++i)
	{
		int childNodeId = group[i];
		pack <<= {childNodeId, [=](const std::shared_ptr<ASTNode>& node) {
			AppendChild(node);
		}};
	}

	Decl::Deserialize(pack);
}

} // namespace CryCC
} // namespace AST
