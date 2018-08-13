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

enum : ResultInterface::slot_type
{
	TESTSET_SET = 1,
};

class TestSet final : public ResultInterface
{
	value_type m_buffer;

public:
	TestSet(size_type init = 0)
	{
		if (init) {
			m_buffer.resize(init);
		}
	}

	void Resize(size_type sz)
	{
		m_buffer.resize(sz);
	}

	size_type Size() const
	{
		return m_buffer.size() + 10;
	}

	value_type& Data()
	{
		return m_buffer;
	}
};

BOOST_AUTO_TEST_CASE(ProgResultSet)
{
	ProgramType prog = Util::MakeProgram();

	// Create set and recast to change local properties.
	{
		ResultInterface& set = prog->ResultSectionSlot<TestSet, TESTSET_SET>();
		BOOST_REQUIRE_EQUAL(10, set.Size());
		dynamic_cast<TestSet&>(set).Resize(20);
	}

	// Access existing set.
	{
		ResultInterface& set = prog->ResultSectionSlot<TestSet, TESTSET_SET>();
		BOOST_REQUIRE_EQUAL(30, set.Size());
	}

	// Create new set with constructor.
	{
		ResultInterface& set = prog->ResultSectionSlot<TestSet, TESTSET_SET + 1>(90);
		BOOST_REQUIRE_EQUAL(100, set.Size());
	}

	// Unload set from program memory.
	{
		prog->ResultSectionSlotRelease<TESTSET_SET>();
		ResultInterface& set = prog->ResultSectionSlot<TestSet, TESTSET_SET>();
		BOOST_REQUIRE_EQUAL(10, set.Size());
	}
}

BOOST_AUTO_TEST_CASE(ProgScratchPad)
{
	ProgramType prog = Util::MakeProgram();

	{
		ScratchPad<SCRATCHPAD_1> pad1{ prog };
		pad1 << "test" << "string";
		std::string text = pad1.ToString();
		BOOST_REQUIRE_EQUAL("teststring", text);
	}

	{
		ScratchPad<SCRATCHPAD_1> pad1{ prog };
		BOOST_REQUIRE_EQUAL("teststring", pad1.ToString());
	}
}

BOOST_AUTO_TEST_CASE(ProgAST)
{
	ASTNodeType tree = Util::MakeUnitTree("test");
	ProgramType prog = Util::MakeProgram(tree);
	BOOST_REQUIRE(prog);
	BOOST_REQUIRE_EQUAL(tree->Id(), prog->Ast()->Id());
}

BOOST_AUTO_TEST_SUITE_END()
