// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <CryCC/AST.h>
#include <CryCC/Program.h>

#include <boost/test/unit_test.hpp>

//
// Key         : Prog
// Test        : Program unit test
// Type        : unit
// Description : -.
//

//TODO:
// - Bind
// - Resultset

using namespace CryCC::AST;
using namespace CryCC::Program;

BOOST_AUTO_TEST_SUITE(Program)

BOOST_AUTO_TEST_CASE(ProgBasic)
{
	ProgramType prog = Util::MakeProgram();
	BOOST_REQUIRE(!prog->operator bool());
	BOOST_REQUIRE(!prog->IsLocked());
	BOOST_REQUIRE(!prog->HasSymbols());
	prog->Lock();
	BOOST_REQUIRE(prog->IsLocked());
}

BOOST_AUTO_TEST_CASE(ProgSymbols)
{
	ProgramType prog = Util::MakeProgram();
	auto& symbolMap = prog->FillSymbols();
	symbolMap.insert({ "test", {} });
	symbolMap.insert({ "test2", {} });
	symbolMap.insert({ "test3", {} });
	BOOST_REQUIRE(prog->HasSymbols());
	BOOST_REQUIRE(prog->HasSymbol("test2"));
	BOOST_REQUIRE_EQUAL(1, prog->SymbolCount());
}

BOOST_AUTO_TEST_CASE(ProgAST)
{
	ASTNodeType tree = Util::MakeUnitTree("test");
	ProgramType prog = Util::MakeProgram(tree);
	BOOST_REQUIRE(prog);
	BOOST_REQUIRE_EQUAL(tree->Id(), prog->Ast()->Id());
}

BOOST_AUTO_TEST_SUITE_END()
