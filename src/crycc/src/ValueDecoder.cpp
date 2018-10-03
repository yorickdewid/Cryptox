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
Value2 ValueCategoryDeserialise(Cry::ByteArray& buffer)
{
	// Convert stream to type.
	Typedef::TypeFacade type;
	Typedef::TypeFacade::Deserialize(type, buffer);

	// Deserialize via value proxy.
	const int identifier = static_cast<int>(buffer.Deserialize<Cry::Byte>());
	switch (identifier)
	{
	case NilValue::value_category_identifier:
	{
		return Value2{ std::move(type), NilValue{} };
	}
	//TODO:
	//case ReferenceValue::value_category_identifier:
	//{
	//	//
	//}
	//TODO:
	//case PointerValue::value_category_identifier:
	//{
	//	//
	//}
	case BuiltinValue::value_category_identifier:
	{
		//TODO: BuiltinValue{ buffer };
		return Value2{ std::move(type), BuiltinValue{ 10 } };
	}
	case ArrayValue::value_category_identifier:
	{
		std::vector<int> v{ 12,34,456 };
		//TODO: ArrayValue{ buffer };
		return Value2{ std::move(type), ArrayValue{ v.cbegin(), v.cend() } };
	}
	case RecordValue::value_category_identifier:
	{
		//return Value2{ std::move(type), RecordValue{ buffer } };
	}
	}

	CryImplExcept(); //TODO:
}

} // namespace CryCC::SubValue::Valuedef::Detail
