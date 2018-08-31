// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "../../modstub/example/mod_example2/MockInterface.h"

#include <Cry/Cry.h>
#include <Cry/Config.h>
#include <Cry/Loader.h>

#include <boost/test/unit_test.hpp>

//
// Key         : ModLoad
// Test        : Unit tests for Cry Module Loader
// Type        : unit
// Description : The module loader loads external modules either with
//               specific name or an entire directory.
//

using namespace Cry::Module;

BOOST_AUTO_TEST_SUITE(CryLoader)

// Load a single module.
BOOST_AUTO_TEST_CASE(ModLoadBasic)
{
	Module<> mod = LoadSingle(DIST_BINARY_DIR "/mod_example1.dll", ANY_COMPONENT_ID);
	mod.Load();
	BOOST_REQUIRE_EQUAL("mod_example1", mod.Name());
	BOOST_REQUIRE_EQUAL("Blub Corp.", mod.Author());
	BOOST_REQUIRE_EQUAL("Copyright (c) 2018", mod.Copyright());
	mod.Unload();
}

// Load all modules from a directory matching an interface.
BOOST_AUTO_TEST_CASE(ModLoadComponent)
{
	std::string testString = "somertesttextstring";
	auto mockMods = Load<MockInterface>(DIST_BINARY_DIR);
	ForEach(mockMods, [&testString](Module<MockInterface>& mockMod) {
		mockMod->Transform(testString);
	});
	BOOST_REQUIRE_EQUAL("SOMERTESTTEXTSTRING", testString);
}

#if 0
// Load all modules from a recursive directory matching an interface.
BOOST_AUTO_TEST_CASE(ModLoadRecursive)
{
	std::string testString = "somertesttextstring";
	auto mockMods = LoadRecursive<MockInterface>(BINARY_DIR);
	ForEach(mockMods, [&testString](Module<MockInterface>& mockMod) {
		mockMod->Transform(testString);
	});
	BOOST_REQUIRE_EQUAL("SOMERTESTTEXTSTRING", testString);
}

BOOST_AUTO_TEST_CASE(ModLoadMultiDir)
{
	std::string testString = "somertesttextstring";
	auto mockMods = LoadRecursive<MockInterface>(BINARY_DIR);
	ForEach(mockMods, [&testString](Module<MockInterface>& mockMod) {
	mockMod->Transform(testString);
	});
	BOOST_REQUIRE_EQUAL("SOMERTESTTEXTSTRING", testString);
}
#endif

BOOST_AUTO_TEST_SUITE_END()
