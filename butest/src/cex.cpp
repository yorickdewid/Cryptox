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
#include <boost/algorithm/string.hpp>

constexpr const char cexTestFileName[] = "_testfile.cex";

struct CEXEnvironment
{
	~CEXEnvironment()
	{
		// Cleanup any left over test files
		boost::filesystem::remove(cexTestFileName);
	}
};

BOOST_FIXTURE_TEST_SUITE(CEX, CEXEnvironment)

BOOST_AUTO_TEST_CASE(WriteToCexFile)
{
	CryExe::Executable exec{ cexTestFileName, CryExe::FileMode::FM_NEW };
	BOOST_CHECK(exec.IsOpen());
	exec.SetOption(CryExe::Executable::Option::OPT_BINREP
				   | CryExe::Executable::Option::OPT_READONLY);

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
		CryExe::Executable exec{ cexTestFileName, CryExe::FileMode::FM_NEW };
	}

	{
		CryExe::Executable exec{ cexTestFileName , CryExe::FileMode::FM_OPEN };

		// Request meta info
		BOOST_REQUIRE(CryExe::Meta::ImageVersion(exec) == (std::make_pair<short, short>(0, 3)));
		BOOST_REQUIRE_EQUAL(CryExe::Meta::ProgramVersion(exec), "");
	}


}

BOOST_AUTO_TEST_CASE(ReadDynamicLibraryImage)
{
	{
		CryExe::DynamicLibrary dll{ cexTestFileName, CryExe::FileMode::FM_NEW };
	}

	{
		CryExe::DynamicLibrary dll{ cexTestFileName, CryExe::FileMode::FM_OPEN };
		BOOST_REQUIRE(dll.IsDynamicLibrary());
	}
}

BOOST_AUTO_TEST_CASE(CreateCexWithSectionFile)
{
	CryExe::Executable exec{ cexTestFileName, CryExe::FileMode::FM_NEW };

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
#define OpenCexWithSectionFile_BYTEARRAY { 0x12, 0xef, 0x88, 0x56, 0x16 }

	{
		CryExe::Executable exec{ cexTestFileName, CryExe::FileMode::FM_NEW };

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

			const CryExe::ByteArray bArray = OpenCexWithSectionFile_BYTEARRAY;
			resSection.Emplace(std::move(bArray));
			resSection << 0x87;

			// Add a resource section
			exec.AddSection(&resSection);
		}

		{
			// Section can only be used once
			CryExe::Section resSection2{ CryExe::Section::SectionType::RESOURCE };
			BOOST_CHECK_THROW(exec.AddSection(&resSection2), std::runtime_error);
		}
	}

	{
		CryExe::Executable exec{ cexTestFileName, CryExe::FileMode::FM_OPEN };
		BOOST_CHECK(exec.IsSealed());

		auto sections = exec.Sections();
		auto it = std::find_if(sections.begin(), sections.end(), [](const CryExe::Section& section)
		{
			return section.Type() == CryExe::Section::SectionType::RESOURCE;
		});
		BOOST_REQUIRE(it != sections.cend());

		// Found our section, should be empty
		BOOST_REQUIRE(it->Empty());

		// Retrieve data from image into section
		exec.GetSectionDataFromImage((*it));
		BOOST_REQUIRE(!it->Empty());
		
		CryExe::ByteArray bArray = OpenCexWithSectionFile_BYTEARRAY;
		bArray.push_back(0x87);
		BOOST_REQUIRE(std::equal(it->Data().cbegin(), it->Data().cend(), bArray.cbegin()));
		it->Clear();
		BOOST_CHECK(it->Empty());
	}

	{
		CryExe::Executable exec{ cexTestFileName, CryExe::FileMode::FM_OPEN };
		auto it = exec.FindSection(CryExe::Section::SectionType::NOTE);

		exec.GetSectionDataFromImage((*it));
		CryExe::ByteArray sArray = it->Data();
		std::string str{ sArray.cbegin(), sArray.cend() };

		BOOST_REQUIRE(boost::algorithm::ends_with(str, "and last"));
		it->Clear();
	}
}

BOOST_AUTO_TEST_SUITE_END()
