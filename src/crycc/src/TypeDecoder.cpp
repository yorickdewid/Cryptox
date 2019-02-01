// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <CryCC/SubValue/Typedef.h>

#include <CryCC/SubValue/BuiltinType.h>
//#include <CryCC/SubValue/OffsetValue.h>
//#include <CryCC/SubValue/NilValue.h>
//#include <CryCC/SubValue/ReferenceValue.h>
//#include <CryCC/SubValue/PointerValue.h>
//#include <CryCC/SubValue/BuiltinValue.h>
#include <CryCC/SubValue/ArrayType.h>
//#include <CryCC/SubValue/RecordValue.h>

namespace CryCC::SubValue::Typedef
{

// Forward declaration for type variant deserialization.
AbstractType::buffer_type& operator>>(AbstractType::buffer_type& os, TypeVariation& type);

// Construct a type from the serialized byte array. The operation
// is only to be used from the type class object and must supply
// an serialized byte array created by the type proxy serializer.
//
// NOTE: Any new type category should be apended to the swtich
//       case below.
InternalBaseType AbstractType::TypeCategoryDeserialise(buffer_type& buffer)
{
	// Convert stream to type.
	TypeVariation type;
	buffer >> type;

	// Identify the type category and then pass the byte array as constructor
	// parameter. This lets the type category itself determine how to rebuild
	// the values from the within.
	switch (type)
	{
	case TypeVariation::BUILTIN:
		return std::make_shared<BuiltinType>(buffer);
	case TypeVariation::RECORD:
		break;
	case TypeVariation::TYPEDEF:
		break;
	case TypeVariation::VARIADIC:
		break;
	case TypeVariation::POINTER:
		break;
	case TypeVariation::ARRAY:
		return std::make_shared<ArrayType>(buffer);
	case TypeVariation::VARIANT:
		break;
	case TypeVariation::NIL:
		break;
	default:
		break;
	}

	CryImplExcept(); //TODO:
}

} // namespace CryCC::SubValue::Typedef
