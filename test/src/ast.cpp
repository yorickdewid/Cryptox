// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <CryCC/AST.h>

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
// - Iterator
// - Serialize/Deserialize
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
