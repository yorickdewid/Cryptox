// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "pfbase.h"

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

BOOST_AUTO_TEST_SUITE(Project)

// Custom deleter in order to remove file after
// project has gone out of scope
auto del = [](ProjectBase::Project *project) {
	auto filename = project->Name();

	// Dealloc project
	delete project;

	// Remove project file
	boost::filesystem::remove(filename);
};

BOOST_AUTO_TEST_CASE(CreateProject)
{
	{
		// Create new project with custom alloc
		std::unique_ptr<ProjectBase::Project, decltype(del)> p{ new ProjectBase::Project{"_testfile.cryx"}, del };

		BOOST_CHECK(p->StoreCount() == 0);
		BOOST_REQUIRE_EQUAL(p->Name(), "_testfile.cryx");
		BOOST_REQUIRE_EQUAL(p->CreateTimestamp(), p->UpdateTimestamp());
	}

	{
		// Create new project with meta and custom alloc
		std::unique_ptr<ProjectBase::Project, decltype(del)> p{ new ProjectBase::Project{ "_testfile.cryx", ProjectBase::MetaData{
			"My Awesome Project",
			"Boost.Test"
		}}, del };

		BOOST_REQUIRE_EQUAL(p->ProjectName(), "My Awesome Project");
		BOOST_REQUIRE_EQUAL(p->Author(), "Boost.Test");
	}

	{
		// Create new project with meta and custom alloc
		std::unique_ptr<ProjectBase::Project> p{ new ProjectBase::Project{ "_testfile.cryx", ProjectBase::MetaData{
			"Second Awesome Project",
			"Cryptox"
		} } };

		p->Save();
		p->Close();
	}

	{
		// Load file directly from filename
		auto p = ProjectBase::Project::LoadFile("_testfile.cryx");

		BOOST_REQUIRE_EQUAL(p->ProjectName(), "Second Awesome Project");
		BOOST_REQUIRE_EQUAL(p->Author(), "Cryptox");
		BOOST_REQUIRE_GT(p->CreateTimestamp(), 0);
		BOOST_REQUIRE_GT(p->UpdateTimestamp(), 0);
	}

	// Remove test file from the last testcase.
	boost::filesystem::remove("_testfile.cryx");
}

BOOST_AUTO_TEST_CASE(CreateStores)
{
	// Create new project with custom alloc
	std::unique_ptr<ProjectBase::Project, decltype(del)> p{ new ProjectBase::Project{ "_testfile.cryx" }, del };

	p->AddStore<ProjectBase::MaterialStore>("material");
	p->AddStore<ProjectBase::DiagramStore>("diagram");

	BOOST_REQUIRE(p->StoreCount() == 2);
	BOOST_REQUIRE_EQUAL(p->Stores().front(), "diagram");
	BOOST_REQUIRE_EQUAL(p->Stores().back(), "material");
}

BOOST_AUTO_TEST_CASE(AddFilesToStore)
{
	// Create new project with custom alloc
	std::unique_ptr<ProjectBase::Project, decltype(del)> p{ new ProjectBase::Project{ "_testfile.cryx" }, del };

	p->AddStore<ProjectBase::MaterialStore>("mat");
	p->AddStore<ProjectBase::DiagramStore>("diag");

	p->GetStore<ProjectBase::DiagramStore>("diag")->AddFile(ProjectBase::File("file1") << "some content");
	p->GetStore<ProjectBase::DiagramStore>("diag")->AddFile(ProjectBase::File("file2") << "extra content");
	p->GetStore<ProjectBase::DiagramStore>("diag")->AddFile(ProjectBase::File("file3") << "fill content");
	p->GetStore<ProjectBase::DiagramStore>("diag")->AddFile(ProjectBase::File{ "file.dia", R"(<?xml version="1.0" ?>)" });

	BOOST_REQUIRE(p->StoreCount() == 2);
	BOOST_REQUIRE(p->GetStore<ProjectBase::DiagramStore>("diag")->Size() == 4);
	BOOST_REQUIRE_EQUAL(p->GetStore<ProjectBase::DiagramStore>("diag")->GetFile("file1").Name(), "file1");
	BOOST_REQUIRE_EQUAL(p->GetStore<ProjectBase::DiagramStore>("diag")->GetFile("file2").Data(), "extra content");
	BOOST_REQUIRE_EQUAL(p->GetStore<ProjectBase::DiagramStore>("diag")->GetFile("file.dia").Data(), "<?xml version=\"1.0\" ?>");

	p->GetStore<ProjectBase::DiagramStore>("diag")->DeleteFile("file3");

	BOOST_REQUIRE(p->GetStore<ProjectBase::DiagramStore>("diag")->Size() == 3);
}

BOOST_AUTO_TEST_CASE(AddKeysToStore)
{
	// Create new project with custom alloc
	std::unique_ptr<ProjectBase::Project, decltype(del)> p{ new ProjectBase::Project{ "_testfile.cryx" }, del };

	p->AddStore<ProjectBase::MaterialStore>("mat");
	p->AddStore<ProjectBase::MaterialStore>("mat2");
	p->AddStore<ProjectBase::MaterialStore>("mat3");
	p->AddStore<ProjectBase::DiagramStore>("diag");

	p->GetStore<ProjectBase::MaterialStore>("mat2")->AddKeypair("key1", "Curve22519", "\x1\x4\x5\x12\x4", "\x7\x78\x12\x1\x4\x71");
	p->GetStore<ProjectBase::MaterialStore>("mat3")->AddKeypair(ProjectBase::Keypair{ "key2", "RSA", std::make_pair("\x1\x4\x5\x12\x5","\x56\x92\x12\x9\x81\x8") });

	BOOST_REQUIRE(p->StoreCount() == 4);
	BOOST_REQUIRE(p->GetStore<ProjectBase::MaterialStore>("mat2")->Size() == 1);
	BOOST_REQUIRE(p->GetStore<ProjectBase::MaterialStore>("mat3")->Size() == 1);
	BOOST_REQUIRE_EQUAL(p->GetStore<ProjectBase::MaterialStore>("mat2")->GetKeypair("key1").Algorithm(), "Curve22519");
	BOOST_REQUIRE_EQUAL(p->GetStore<ProjectBase::MaterialStore>("mat3")->GetKeypair("key2").Algorithm(), "RSA");

	p->GetStore<ProjectBase::MaterialStore>("mat2")->DeleteKeypair("key1");

	BOOST_REQUIRE(p->GetStore<ProjectBase::MaterialStore>("mat2")->Size() == 0);
}

BOOST_AUTO_TEST_SUITE_END()
