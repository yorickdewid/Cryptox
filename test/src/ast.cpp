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
// - Children
// - Alteration
// - Iterator
// - Serialize/Deserialize
// - User Data

using namespace CryCC::AST;

BOOST_AUTO_TEST_SUITE(AST)

BOOST_AUTO_TEST_CASE(ASTBasicNode)
{
	ASTNodeType node = Util::MakeASTNode<TranslationUnitDecl>("source");
	node->SetLocation(12, 721);

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

	auto compond = Util::MakeASTNode<CompoundStmt>();

	tree->AppendChild(compond);
}

BOOST_AUTO_TEST_CASE(ASTIterator)
{
	auto tree = Util::MakeUnitTree("source");
	CryCC::AST::AST{ tree };
}

BOOST_AUTO_TEST_SUITE_END()
