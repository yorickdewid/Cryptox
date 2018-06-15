// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "../src/ValueHelper.h"
#include "../src/RecordValue.h"

#include <boost/test/unit_test.hpp>

//
// Test        : Record value unitttest
// Type        : unit
// Description : Test cases for the record value system. The Record
//               value is a special value type for composite structures.
//               The unit test should test most functionality.
//

using namespace CoilCl;

BOOST_AUTO_TEST_SUITE(ValueRecord)

BOOST_AUTO_TEST_CASE(ValRecBasic)
{
	{
		BOOST_REQUIRE_EQUAL(0, Valuedef::RecordValue{}.Size());
	}

	{
		Valuedef::RecordValue record{ "struct" };
		record.EmplaceField("f", Valuedef::RecordValue::AutoValue(Util::MakeInt2(12)));
		
		BOOST_REQUIRE(record.HasRecordName());
		BOOST_REQUIRE_EQUAL(record.Size(), 1);
	}

	{
		auto valInt = Util::MakeInt2(12);
		Valuedef::RecordValue anonRecord;
		anonRecord.EmplaceField("field", Valuedef::RecordValue::AutoValue(valInt));

		BOOST_REQUIRE(!anonRecord.HasRecordName());
		BOOST_REQUIRE_EQUAL(anonRecord.Size(), 1);

		Valuedef::RecordValue anonRecord2;
		anonRecord2.EmplaceField("field", Valuedef::RecordValue::AutoValue(valInt));
		BOOST_REQUIRE_EQUAL(anonRecord, anonRecord2);
	}

	{
		auto valDouble = Util::MakeDouble2(8723.7612);
		Valuedef::RecordValue record{ "testrec" };
		record.EmplaceField("i", Valuedef::RecordValue::AutoValue(valDouble));

		auto valDouble2 = Util::MakeDouble2(81.7213);
		Valuedef::RecordValue record2{ "testrec" };
		record2.EmplaceField("i", Valuedef::RecordValue::AutoValue(valDouble2));
		BOOST_REQUIRE_NE(record, record2);
	}
}

BOOST_AUTO_TEST_CASE(ValRecError)
{
	{
		auto value = Valuedef::RecordValue::AutoValue(Util::MakeInt2(81827));

		Valuedef::RecordValue record{ "record" };
		record.AddField({ "i", value });

		BOOST_REQUIRE_THROW(record.AddField({ "i", value }), Valuedef::RecordValue::FieldExistException);
	}
}

BOOST_AUTO_TEST_CASE(ValRecSerialize)
{
	{
		Cry::ByteArray buffer;
		Valuedef::RecordValue record{ "record" };
		Valuedef::RecordValue::Serialize(record, buffer);

		Valuedef::RecordValue record2;
		Valuedef::RecordValue::Deserialize(record2, buffer);
		
		BOOST_REQUIRE_EQUAL(record, record2);
	}

	{
		Cry::ByteArray buffer;
		Valuedef::RecordValue record;
		record.AddField({ "x", Valuedef::RecordValue::AutoValue(Util::MakeInt2(834)) });
		record.AddField({ "y", Valuedef::RecordValue::AutoValue(Util::MakeChar2('Y')) });
		record.AddField({ "z", Valuedef::RecordValue::AutoValue(Util::MakeInt2(0)) });
		Valuedef::RecordValue::Serialize(record, buffer);

		Valuedef::RecordValue record2;
		Valuedef::RecordValue::Deserialize(record2, buffer);

		BOOST_REQUIRE_EQUAL(3, record2.Size());
		//TODO: Enable when value is serializable
		//BOOST_REQUIRE_EQUAL(record, record2);
	}
}

BOOST_AUTO_TEST_SUITE_END()
