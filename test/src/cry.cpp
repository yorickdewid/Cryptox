// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <Cry/Cry.h>
#include <Cry/ByteOrder.h>

#include <boost/test/unit_test.hpp>

//
// Key         : Cry
// Test        : Unit tests for Cry Framework
// Type        : unit
// Description : -
//

BOOST_AUTO_TEST_SUITE(CryCommon)

BOOST_AUTO_TEST_CASE(CryCommonHeaderDefinitions)
{
	{
		BOOST_REQUIRE_EQUAL(CRY_UNASSIGNED, 0);
	}

	{
		struct tm timeinfo;
		CRY_LOCALTIME(&timeinfo);
		BOOST_REQUIRE_GT(timeinfo.tm_year, 100);
	}

	{
		char buffer[1024];
		CRY_MEMZERO(buffer, 1024);
		BOOST_REQUIRE(buffer[0] == '\0' && buffer[1024 - 1] == '\0');
	}

	{
		char buffer[32];
		CRY_MEMSET(buffer, '\7', 32);
		BOOST_REQUIRE(buffer[0] == '\7' && buffer[32 - 1] == '\7');
	}

	{
		char block[] = { '\x34', '\xfe', '\x9a', '\xbb', '\x3c', '\x5e', '\x88', '\x1a' };
		char buffer[8];
		CRY_MEMZERO(buffer, 8);
		CRY_MEMCPY(buffer, 8, block, sizeof(block));
		BOOST_REQUIRE_EQUAL(memcmp(buffer, block, 8), 0);
	}

	{
		char str[] = { "teststring" };
		char buffer[16];
		CRY_MEMZERO(buffer, 16);
		CRY_STRCPY(buffer, 16, str);
		BOOST_REQUIRE_EQUAL(strcmp(buffer, str), 0);
	}
}

BOOST_AUTO_TEST_CASE(CryByteOrder)
{
	{
		uint16_t o = 49531;
		uint16_t i = BSWAP16(o);
		uint16_t e = BSWAP16(i);
		BOOST_REQUIRE_EQUAL(o, e);
	}

	{
		uint32_t o = 3154967261;
		uint32_t i = BSWAP32(o);
		uint32_t e = BSWAP32(i);
		BOOST_REQUIRE_EQUAL(o, e);
	}

	{
		uint64_t o = 17846744073709551617U;
		uint64_t i = BSWAP64(o);
		uint64_t e = BSWAP64(i);
		BOOST_REQUIRE_EQUAL(o, e);
	}
}

BOOST_AUTO_TEST_SUITE_END()
