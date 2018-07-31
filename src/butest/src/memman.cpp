// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <Cry/MemoryManager.h>

#include <boost/test/unit_test.hpp>

//
// Test        : Memory manager unitttest
// Type        : unit
// Description : Test the memory manager.
//

using namespace Cry::MemoryManager;

BOOST_AUTO_TEST_SUITE(MemMan)

BOOST_AUTO_TEST_CASE(MMBasicAlloc)
{
	MemoryManager mgr;

	BOOST_REQUIRE_EQUAL(0, mgr.HeapUsedSize());

	void *block = mgr.HeapAllocate(128);

	BOOST_REQUIRE_EQUAL(256, mgr.HeapUsedSize());

	mgr.HeapFree(block);

	BOOST_REQUIRE_EQUAL(0, mgr.HeapUsedSize());
}

BOOST_AUTO_TEST_CASE(MMBasicOSPool)
{
	MultiPoolMemoryManager mgr{ BackedPool{} };

	void *block = mgr.HeapAllocate(10);
	mgr.HeapFree(block);
}

BOOST_AUTO_TEST_CASE(MMBasicExhaust)
{
	MultiPoolMemoryManager mgr{ ContiguousPool<1280>{} };

	mgr.HeapAllocate(281);
	mgr.HeapAllocate(212);
	mgr.HeapAllocate(10);
	mgr.HeapAllocate(8);

	BOOST_REQUIRE_THROW(mgr.HeapAllocate(421), OutOfMemoryException);
}

BOOST_AUTO_TEST_SUITE_END()
