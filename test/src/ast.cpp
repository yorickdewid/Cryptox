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
// Description : -
//

using namespace CryCC::AST;

BOOST_AUTO_TEST_SUITE(AST)

BOOST_AUTO_TEST_CASE(ASTBasic)
{
	ASTNodeType tree = Util::MakeSyntaxTree("source");

	BOOST_REQUIRE(NodeID::TRANSLATION_UNIT_DECL_ID == tree->Label());
}

BOOST_AUTO_TEST_SUITE_END()
