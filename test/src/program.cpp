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
	ASTNodeType tree = Util::MakeUnitTree("test");
	ProgramType prog = Util::MakeProgram();
	SymbolMap& symbolMap = prog->SymbolTable();
	symbolMap.Insert({ "test", {} });
	symbolMap.Insert({ "test2", {} });
	symbolMap << SymbolMap::symbol_type{ "test3", {} };
	symbolMap << SymbolMap::symbol_type{ "test4", tree };
	BOOST_REQUIRE(prog->HasSymbols());
	BOOST_REQUIRE(prog->SymbolTable().Contains("test2"));
	BOOST_REQUIRE_EQUAL(1, prog->SymbolCount());
	BOOST_REQUIRE_EQUAL(nullptr, prog->SymbolTable().GetNode("test3"));
	BOOST_REQUIRE_EQUAL(tree, prog->SymbolTable().GetNode("test4"));
}

BOOST_AUTO_TEST_CASE(ProgAST)
{
	ASTNodeType tree = Util::MakeUnitTree("test");
	ProgramType prog = Util::MakeProgram(tree);
	BOOST_REQUIRE(prog);
	BOOST_REQUIRE_EQUAL(tree->Id(), prog->Ast()->Id());
}

BOOST_AUTO_TEST_SUITE_END()
