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

		// Request meta info
		BOOST_REQUIRE(CryExe::Meta::ImageVersion(exec) == (std::make_pair<short, short>(0, 3)));
		BOOST_REQUIRE_EQUAL(CryExe::Meta::ProgramVersion(exec), "");
	}
}

BOOST_AUTO_TEST_CASE(CreateCexWithSectionFile)
{
	CryExe::Executable exec{ cexTestFileName , CryExe::FileMode::FM_NEW };

	// Create native section
	std::unique_ptr<CryExe::Section> textSection = std::make_unique<CryExe::Section>(CryExe::Section::SectionType::NATIVE);
	textSection->Emplace({ 0x2e, 0x21, 0xb6, 0x45, 0x09 });

	// Add a native section
	exec.AddSection(textSection.get());

	// Create note section
	std::unique_ptr<CryExe::Section> noteSection = std::make_unique<CryExe::Section>(CryExe::Section::SectionType::NOTE);
	noteSection->Emplace("note test");

	// Add a note section
	exec.AddSection(noteSection.get());

	CryExe::Executable::Seal(exec);
}

BOOST_AUTO_TEST_CASE(OpenCexWithSectionFile)
{
	{
		CryExe::Executable exec{ cexTestFileName , CryExe::FileMode::FM_NEW };

		{
			// Create note section
			CryExe::Section noteSection{ CryExe::Section::SectionType::NOTE };
			noteSection.Emplace("test note");
			noteSection << "add";
			noteSection << "testing";
			noteSection << "appended string";
			noteSection += " and last";

			// Add a note section
			exec.AddSection(&noteSection);
		}

		{
			// Create resource section
			CryExe::Section resSection{ CryExe::Section::SectionType::RESOURCE };

			CryExe::ByteArray bArray = { 0x12, 0xef, 0x88, 0x56, 0x16 };
			resSection.Emplace(std::move(bArray));
			resSection << 0x87;

			// Add a resource section
			exec.AddSection(&resSection);
		}
	}

	{
		CryExe::Executable exec{ cexTestFileName , CryExe::FileMode::FM_OPEN };
		BOOST_CHECK(exec.IsSealed());
	}
}

BOOST_AUTO_TEST_SUITE_END()
