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

#ifdef _OBSOLETE_

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
class RecordProxy final
{
	// Create new record field value from record type.
	static Value MemberFromType(const Value& value, const std::string& name)
	{
		const auto recordType = value.Type().DataType<RecordType>();

		// Find the type field matching the field name.
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
	static void AssignNewRecord(Value& value, RecordValue&& record)
	{
		const auto recordType = value.Type().DataType<RecordType>();

		// Assign record value to passed record.
		Value newValue = (recordType->TypeSpecifier() == RecordType::Specifier::STRUCT)
			? Util::MakeStruct(std::move(record))
			: Util::MakeUnion(std::move(record));
		value = newValue;
	}

public:
	// A pointer to the value is returned to the caller so that the inner record field
	// value van be altered outside the scope of the record.
	static std::shared_ptr<Value> MemberValue(Value& value, const std::string& name)
	{
		// Test if value holds a record value.
		if (!value.Empty()) {
			RecordValue recVal = value.As<RecordValue>();
			if (recVal.HasField(name)) {
				return recVal.GetField(name);
			}
			else {
				auto memberValue = RecordValue::AutoValue(MemberFromType(value, name));
				recVal.AddField({ name, memberValue });
				AssignNewRecord(value, std::move(recVal));
				return memberValue;
			}
		}
		// Create a new record value.
		else {
			RecordValue record;
			auto memberValue = RecordValue::AutoValue(MemberFromType(value, name));
			record.AddField({ name, memberValue });
			AssignNewRecord(value, std::move(record));
			return memberValue;
		}
	}
};

std::shared_ptr<Value> RecordMemberValue(Value& value, const std::string& name)
{
	return RecordProxy::MemberValue(value, name);
}

#endif // _OBSOLETE_

} // namespace Valuedef
} // namespace SubValue
} // namespace CryCC
