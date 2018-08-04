// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <CryCC/SubValue.h>

#include <boost/test/unit_test.hpp>

//
// Key         : ValRec
// Test        : Record value unitttest
// Type        : unit
// Description : Test cases for the record value system. The Record
//               value is a special value type for composite structures.
//               The unit test should test most functionality.
//

using namespace CryCC::SubValue::Valuedef;

BOOST_AUTO_TEST_SUITE(ValueRecord)

BOOST_AUTO_TEST_CASE(ValRecBasic)
{
	{
		BOOST_REQUIRE_EQUAL(0, RecordValue{}.Size());
	}

	{
		RecordValue record{ "struct" };
		record.EmplaceField("f", RecordValue::AutoValue(Util::MakeInt(12)));
		
		BOOST_REQUIRE(record.HasRecordName());
		BOOST_REQUIRE_EQUAL(record.Size(), 1);
	}

	{
		auto valInt = Util::MakeInt(12);
		RecordValue anonRecord;
		anonRecord.EmplaceField("field", RecordValue::AutoValue(valInt));

		BOOST_REQUIRE(!anonRecord.HasRecordName());
		BOOST_REQUIRE_EQUAL(anonRecord.Size(), 1);

		RecordValue anonRecord2;
		anonRecord2.EmplaceField("field", RecordValue::AutoValue(valInt));
		BOOST_REQUIRE_EQUAL(anonRecord, anonRecord2);
	}

	{
		auto valDouble = Util::MakeDouble(8723.7612);
		RecordValue record{ "testrec" };
		record.EmplaceField("i", RecordValue::AutoValue(valDouble));

		auto valDouble2 = Util::MakeDouble(81.7213);
		RecordValue record2{ "testrec" };
		record2.EmplaceField("i", RecordValue::AutoValue(valDouble2));
		BOOST_REQUIRE_NE(record, record2);
	}
}

BOOST_AUTO_TEST_CASE(ValRecError)
{
	{
		auto value = RecordValue::AutoValue(Util::MakeInt(81827));

		RecordValue record{ "record" };
		record.AddField({ "i", value });

		BOOST_REQUIRE_THROW(record.AddField({ "i", value }), RecordValue::FieldExistException);
	}
}

BOOST_AUTO_TEST_CASE(ValRecSerialize)
{
	{
		Cry::ByteArray buffer;
		RecordValue record{ "record" };
		RecordValue::Serialize(record, buffer);

		RecordValue record2;
		RecordValue::Deserialize(record2, buffer);
		
		BOOST_REQUIRE_EQUAL(record, record2);
	}

	{
		Cry::ByteArray buffer;
		RecordValue record;
		record.AddField({ "x", RecordValue::AutoValue(Util::MakeInt(834)) });
		record.AddField({ "y", RecordValue::AutoValue(Util::MakeChar('Y')) });
		record.AddField({ "z", RecordValue::AutoValue(Util::MakeInt(0)) });
		RecordValue::Serialize(record, buffer);

		RecordValue record2;
		RecordValue::Deserialize(record2, buffer);

		BOOST_REQUIRE_EQUAL(3, record2.Size());
		BOOST_REQUIRE_EQUAL(record, record2);
	}
}

BOOST_AUTO_TEST_SUITE_END()
