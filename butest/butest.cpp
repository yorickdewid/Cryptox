#include "pfbase.h"

#define BOOST_TEST_MODULE Project

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

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

	p->GetStore<ProjectBase::DiagramStore>("diag")->AddFile(ProjectBase::File{ "file.dia", "<?xml version=\"1.0\" ?>" });

	BOOST_REQUIRE(p->StoreCount() == 2);
	BOOST_REQUIRE(p->GetStore<ProjectBase::DiagramStore>("diag")->Size() == 4);
}

