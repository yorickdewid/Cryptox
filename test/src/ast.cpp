// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <CryCC/AST.h>

#include <Cry/Algorithm.h>
#include <Cry/Serialize.h>

#include <boost/test/unit_test.hpp>

//
// Key         : AST
// Test        : Abstract Syntax Tree unitttest
// Type        : unit
// Description : Test the abstract syntax tree structure and all
//               the helper methods. The AST is linchpin for the entire
//               program, and thus needs to be tested thoroughly.
//

//TODO:
// - User Data

using namespace CryCC::AST;

BOOST_AUTO_TEST_SUITE(AST)

BOOST_AUTO_TEST_CASE(ASTBasicNode)
{
	ASTNodeType node = Util::MakeASTNode<TranslationUnitDecl>("source");
	node->SetLocation(12, 721);

	BOOST_REQUIRE_LT(0, node->Id());
	BOOST_REQUIRE_EQUAL(0, node->ChildrenCount());
	BOOST_REQUIRE_EQUAL(0, node->ModifierCount());
	BOOST_REQUIRE_EQUAL(12, node->Location().first);
	BOOST_REQUIRE_EQUAL(721, node->Location().second);
	BOOST_REQUIRE(node->Parent().expired());
	BOOST_REQUIRE_NE(std::string::npos, node->NodeName().find("TranslationUnitDecl"));
	BOOST_REQUIRE(NodeID::TRANSLATION_UNIT_DECL_ID == node->Label());
	BOOST_REQUIRE(Util::IsNodeTranslationUnit(node));
}

BOOST_AUTO_TEST_CASE(ASTBasicTree)
{
	auto tree = Util::MakeUnitTree("source");

	auto compond1 = Util::MakeASTNode<CompoundStmt>();
	tree->AppendChild(compond1);
	auto compond2 = Util::MakeASTNode<CompoundStmt>();
	tree->AppendChild(compond2);
	auto stmt = Util::MakeASTNode<ReturnStmt>();
	compond2->AppendChild(stmt);
	BOOST_REQUIRE_EQUAL(2, tree->ChildrenCount());
	BOOST_REQUIRE_EQUAL(tree, compond1->Parent().lock());
	BOOST_REQUIRE_EQUAL(tree->operator[](1).lock(), compond2);
}

BOOST_AUTO_TEST_CASE(ASTEmplaceNode)
{
	auto arg = Util::MakeASTNode<ArgumentStmt>();
	auto param1 = Util::MakeASTNode<ParamStmt>();
	arg->AppendArgument(param1);
	auto param2 = Util::MakeASTNode<ParamStmt>();
	arg->AppendArgument(param2);
	BOOST_REQUIRE(NodeID::PARAM_STMT_ID == arg->Children().front().lock()->Label());
	BOOST_REQUIRE_EQUAL(2, arg->ChildrenCount());
	arg->Emplace(1, Util::MakeASTNode<ReturnStmt>());
	BOOST_REQUIRE(NodeID::RETURN_STMT_ID == arg->Children().back().lock()->Label());
	BOOST_REQUIRE_EQUAL(1, arg->ModifierCount());
}

BOOST_AUTO_TEST_CASE(ASTLiteral)
{
	static const std::string lstr{ "string" };
	auto str = Util::MakeASTNode<StringLiteral>(lstr);
	BOOST_REQUIRE(Util::IsNodeLiteral(str));
	BOOST_REQUIRE_EQUAL(lstr, str->Value().As<std::string>());
}

BOOST_AUTO_TEST_CASE(ASTIterator)
{
	auto tree = Util::MakeUnitTree("source");
	tree->AppendChild(Util::MakeASTNode<VariadicDecl>());
	tree->AppendChild(Util::MakeASTNode<VariadicDecl>());
	tree->AppendChild(Util::MakeASTNode<VariadicDecl>());
	tree->AppendChild(Util::MakeASTNode<VariadicDecl>());
	tree->AppendChild(Util::MakeASTNode<VariadicDecl>());
	tree->AppendChild(Util::MakeASTNode<VariadicDecl>());

	CryCC::AST::AST ast{ tree };
	for (auto it = ast.cbegin(); it < ast.cend(); ++it) {
		if (it->Label() == NodeID::TRANSLATION_UNIT_DECL_ID) {
			continue;
		}
		BOOST_REQUIRE(it->Label() == NodeID::VARIADIC_DECL_ID);
	}
}

class NodePacker final : public Serializable::Interface
{
	Cry::ByteArray buffer;
	std::map<IdType, std::function<void(const std::shared_ptr<ASTNode>&)>> callbacks;

	class Group : public Serializable::ChildGroupInterface
	{
		std::vector<std::shared_ptr<ASTNode>> m_astList;

		void SaveNode(std::shared_ptr<ASTNode>& node)
		{
			m_astList.push_back(node);
		}

		void SaveNode(nullptr_t)
		{
			// Not implemented.
		}

		int LoadNode(int i)
		{
			return m_astList.at(i)->Id();
		}

		void SetSize(size_t)
		{
			// Not implemented.
		}

		size_t GetSize() noexcept
		{
			return m_astList.size();
		}
	};

	Serializable::GroupListType m_groups;

public:
	Serializable::GroupListType CreateChildGroups(size_t size)
	{
		for (size_t i = 0; i < size; ++i) {
			m_groups.push_back(std::make_shared<Group>());
		}

		return m_groups;
	}

	Serializable::GroupListType GetChildGroups()
	{
		return m_groups;
	}

	NodeID GetNodeId()
	{
		auto offset = buffer.Offset();
		auto v = static_cast<NodeID>(buffer.Deserialize<Cry::Byte>());
		buffer.StartOffset(offset);
		return v;
	}

	void FireDependencies(std::shared_ptr<ASTNode>& node)
	{
		auto it = callbacks.find(node->Id());
		if (it != callbacks.end()) {
			callbacks[node->Id()](node);
		}
	}

	virtual void operator<<(int v) { buffer.Serialize(static_cast<Cry::DoubleWord>(v)); }
	virtual void operator<<(double v) { buffer.Serialize(static_cast<Cry::DoubleWord>(v)); }
	virtual void operator<<(bool v) { buffer.Serialize(static_cast<Cry::Byte>(v)); }
	virtual void operator<<(NodeID v) { buffer.Serialize(static_cast<Cry::Byte>(v)); }
	virtual void operator<<(std::string) {}
	virtual void operator<<(std::vector<uint8_t>) {}

	virtual void operator>>(int& v) { v = static_cast<int>(buffer.Deserialize<Cry::DoubleWord>()); }
	virtual void operator>>(double& v) { v = static_cast<double>(buffer.Deserialize<Cry::DoubleWord>()); }
	virtual void operator>>(bool& v) { v = static_cast<bool>(buffer.Deserialize<Cry::Byte>()); }
	virtual void operator>>(NodeID& v) { v = static_cast<NodeID>(buffer.Deserialize<Cry::Byte>()); }
	virtual void operator>>(std::string&) {}
	virtual void operator>>(std::vector<uint8_t>&) {}

	virtual void operator<<=(std::pair<IdType, std::function<void(const std::shared_ptr<ASTNode>&)>> cb)
	{
		callbacks.insert(cb);
	}
};

BOOST_AUTO_TEST_CASE(ASTSerialize)
{
	auto param = Util::MakeASTNode<ParamStmt>();
	auto brk1 = Util::MakeASTNode<BreakStmt>();
	param->AppendParamter(brk1);
	auto brk2 = Util::MakeASTNode<BreakStmt>();
	param->AppendParamter(brk2);

	NodePacker packer;
	param->Serialize(packer);
	brk1->Serialize(packer);
	brk2->Serialize(packer);

	auto param2_node = ASTFactory::MakeNode(&packer);
	auto brk12_node = ASTFactory::MakeNode(&packer);
	auto brk22_node = ASTFactory::MakeNode(&packer);
	auto param2 = Util::NodeCast<ParamStmt>(param2_node);
	auto brk12 = Util::NodeCast<BreakStmt>(brk12_node);
	auto brk22 = Util::NodeCast<BreakStmt>(brk22_node);
	BOOST_REQUIRE((*param.get()) == (*param2.get()));
	BOOST_REQUIRE((*brk1.get()) == (*brk12.get()));
	BOOST_REQUIRE((*brk2.get()) == (*brk22.get()));
}

BOOST_AUTO_TEST_CASE(ASTMisc)
{
	std::shared_ptr<CompoundStmt> compond = Util::MakeASTNode<CompoundStmt>();
	std::shared_ptr<FunctionDecl> func = Util::MakeASTNode<FunctionDecl>("func", compond);
	ASTNodeType breakNode = Util::MakeASTNode<BreakStmt>();

	BOOST_REQUIRE_EQUAL(Util::NodeCast<BreakStmt>(breakNode)->Id(), breakNode->Id());
	BOOST_REQUIRE(Util::IsNodeCompound(compond));
	BOOST_REQUIRE(Util::IsNodeFunction(func));
}

BOOST_AUTO_TEST_SUITE_END()
