#include "pfbase.h"

#define BOOST_TEST_MODULE Project

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

BOOST_AUTO_TEST_CASE(CreateProject)
{
	auto del = [](ProjectBase::Project *project) {
		auto filename = project->Name();
		
		// Dealloc project
		delete project;
		
		// Remove project file
		boost::filesystem::remove(filename);
	};

	// Create new project with custom alloc
	std::unique_ptr<ProjectBase::Project, decltype(del)> p{ new ProjectBase::Project{"_testfile.cryx"}, del };

	BOOST_CHECK(p->StoreCount() == 0);
	BOOST_REQUIRE_EQUAL(p->Name(), "_testfile.cryx");
	BOOST_REQUIRE_EQUAL(p->CreateTimestamp(), p->UpdateTimestamp());
}

BOOST_AUTO_TEST_CASE(CreateStores)
{
	auto del = [](ProjectBase::Project *project) {
		auto filename = project->Name();

		// Dealloc project
		delete project;

		// Remove project file
		boost::filesystem::remove(filename);
	};

	// Create new project with custom alloc
	std::unique_ptr<ProjectBase::Project, decltype(del)> p{ new ProjectBase::Project{ "_testfile.cryx" }, del };

	p->AddStore<ProjectBase::MaterialStore>("material");
	p->AddStore<ProjectBase::DiagramStore>("diagram");

	BOOST_REQUIRE(p->StoreCount() == 2);
}

