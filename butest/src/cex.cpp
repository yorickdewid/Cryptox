// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "cex.h"

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

constexpr const char cexTestFileName[] = "_testfile.cex";

struct CEXEnvironment
{
	CEXEnvironment()
	{
		//
	}

	~CEXEnvironment()
	{
		// Cleanup any left over test files
		boost::filesystem::remove(cexTestFileName);
	}
};

BOOST_FIXTURE_TEST_SUITE(CEX, CEXEnvironment)

BOOST_AUTO_TEST_CASE(WriteToCexFile)
{
	CryExe::Executable exec{ cexTestFileName , CryExe::FileMode::FM_NEW };
	BOOST_CHECK(exec.IsOpen());

	// Flush to disk, FWIW
	exec.Flush();

	// Check on readonly sealed image
	const CryExe::Executable& execSeal = CryExe::Executable::Seal(exec);
	BOOST_REQUIRE(execSeal.Size() > 0);
	BOOST_REQUIRE_EQUAL(execSeal.Name(), cexTestFileName);
}

BOOST_AUTO_TEST_CASE(ReadToCexFile)
{
	{
		CryExe::Executable exec{ cexTestFileName , CryExe::FileMode::FM_NEW };
	}

	{
		CryExe::Executable exec{ cexTestFileName , CryExe::FileMode::FM_OPEN };

		BOOST_REQUIRE_EQUAL(CryExe::Meta::ImageVersion(exec), "");
		BOOST_REQUIRE_EQUAL(CryExe::Meta::ProgramVersion(exec), "");
	}
}

BOOST_AUTO_TEST_CASE(CreateCexWithSectionFile)
{
	CryExe::Executable exec{ cexTestFileName , CryExe::FileMode::FM_NEW };

	// Create .text section
	std::unique_ptr<CryExe::Section> textSection = std::make_unique<CryExe::Section>();
	textSection->Emplace({ 0x2e, 0x21, 0xb6, 0x45, 0x09 });

	// Add a text section
	exec.AddSection(textSection.release());

	// Create .note section
	std::unique_ptr<CryExe::Section> noteSection = std::make_unique<CryExe::Section>();
	noteSection->Emplace("test note");

	// Add a note section
	exec.AddSection(noteSection.release());

	CryExe::Executable::Seal(exec);
}

BOOST_AUTO_TEST_SUITE_END()
