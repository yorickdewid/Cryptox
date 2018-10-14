// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <CryCC/SubValue/OffsetValue.h>
#include <CryCC/SubValue/NilValue.h>
#include <CryCC/SubValue/ReferenceValue.h>
#include <CryCC/SubValue/PointerValue.h>
#include <CryCC/SubValue/BuiltinValue.h>
#include <CryCC/SubValue/ArrayValue.h>
#include <CryCC/SubValue/RecordValue.h>

namespace CryCC::SubValue::Valuedef::Detail
{

// Construct a value from the serialized byte array. The operation
// is only to be used from the value class object and must supply
// an serialized byte array created by the value proxy serializer.
//
// NOTE: Any new value category should be apended to the swtich
//       case below.
Value ValueCategoryDeserialise(Cry::ByteArray& buffer)
{
	// Convert stream to type.
	Typedef::TypeFacade type;
	Typedef::TypeFacade::Deserialize(type, buffer);

	// Identify the value category and then pass the byte array as constructor
	// parameter. This lets the value category itself determine how to rebuild
	// the values from the inside.
	const auto identifier = static_cast<int>(buffer.Deserialize<Cry::Byte>());
	switch (identifier)
	{
	case NilValue::value_category_identifier:
		return Value{ std::move(type), NilValue{ buffer } };
	case ReferenceValue::value_category_identifier:
		return Value{ std::move(type), ReferenceValue{ buffer } };
	case PointerValue::value_category_identifier:
		return Value{ std::move(type), PointerValue{ buffer } };
	case BuiltinValue::value_category_identifier:
		return Value{ std::move(type), BuiltinValue{ buffer } };
	case ArrayValue::value_category_identifier:
		return Value{ std::move(type), ArrayValue{ buffer } };
	case RecordValue::value_category_identifier:
		return Value{ std::move(type), RecordValue{ buffer } };
	}

	CryImplExcept(); //TODO:
}

} // namespace CryCC::SubValue::Valuedef::Detail
