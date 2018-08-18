// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <CryCC/SubValue/RecordValue.h>
#include <CryCC/SubValue/Valuedef.h>
#include <CryCC/SubValue/ValueHelper.h>

namespace CryCC
{
namespace SubValue
{
namespace Valuedef
{

using namespace CryCC::SubValue::Typedef;

// The record proxy implements a assignment-on-access pattern.
//
// Access record as a single entity value, and do either of the following:
//   1.) If the record value is not initialized, create a new record value and assign
//       it to the value. Then continue:
//   2.) If the member values does not exist in the record value, create a new member value, add
//       it to the record value and return the member value as writable value.
//   3.) If the member value already exist in the record value, return the value immediately.
//
// Since the value type (record type) holds the record structure in the type definition, the
// record value can be constructed from this and the operations below use the type as a strucutre
// template.
class RecordProxy
{
	// Create new record field value from record type.
	static Valuedef::Value MemberFromType(const Valuedef::Value& recordValue, const std::string& name)
	{
		const auto recordType = recordValue.Type().DataType<RecordType>();

		const auto fields = recordType->Fields();
		auto it = std::find_if(fields.cbegin(), fields.cend(), [=](auto pair) {
			return name == pair.first;
		});
		if (it == fields.cend()) {
			CryImplExcept(); //TODO: 'recordType->Name()' has no member named 'member->FieldName()'
		}

		// Create new value from type definition.
		return (*it->second.get());
	}

	// Create a new record value and assign it to the passed value. This initializes the value as a record. If the 
	// record type was setup with a name, copy the name to the record value.
	static void AssignNewRecord(Valuedef::Value& recordValue, Valuedef::RecordValue&& record)
	{
		const auto recordType = recordValue.Type().DataType<RecordType>();

		// Set record name if known.
		if (!recordType->IsAnonymous()) {
			if (!record.HasRecordName()) {
				record.SetRecordName(recordType->Name());
			}
		}

		// Assign record value to passed record.
		Valuedef::Value newValue = (recordType->TypeSpecifier() == RecordType::Specifier::STRUCT)
			? Util::MakeStruct(std::move(record))
			: Util::MakeUnion(std::move(record));
		recordValue = newValue;
	}

public:
	static Valuedef::Value MemberValue(Valuedef::Value& recordValue, const std::string& name)
	{
		// Test if value holds a record value.
		if (!recordValue.Empty()) {
			Valuedef::RecordValue recVal = recordValue.As<Valuedef::RecordValue>();
			if (recVal.HasField(name)) {
				return (*recVal.GetField(name));
			}
			else {
				auto memberValue = MemberFromType(recordValue, name);
				recVal.AddField({ name, Valuedef::RecordValue::AutoValue(memberValue) });
				AssignNewRecord(recordValue, std::move(recVal));
				return memberValue;
			}
		}
		// Create a new record value.
		else {
			Valuedef::RecordValue recVal;
			auto memberValue = MemberFromType(recordValue, name);
			recVal.AddField({ name, Valuedef::RecordValue::AutoValue(memberValue) });
			AssignNewRecord(recordValue, std::move(recVal));
			return memberValue;
		}
	}
};

} // namespace Valuedef
} // namespace SubValue
} // namespace CryCC
