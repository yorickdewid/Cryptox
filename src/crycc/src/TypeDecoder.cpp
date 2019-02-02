// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <CryCC/SubValue/Typedef.h>

#include <CryCC/SubValue/TypedefType.h>
#include <CryCC/SubValue/NilType.h>
#include <CryCC/SubValue/VariantType.h>
#include <CryCC/SubValue/VariadicType.h>
#include <CryCC/SubValue/PointerType.h>
#include <CryCC/SubValue/BuiltinType.h>
#include <CryCC/SubValue/ArrayType.h>
#include <CryCC/SubValue/RecordType.h>

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
		return std::make_shared<RecordType>(buffer);
	case TypeVariation::TYPEDEF:
		return std::make_shared<TypedefType>(buffer);
	case TypeVariation::VARIADIC:
		return std::make_shared<VariadicType>(buffer);
	case TypeVariation::POINTER:
		return std::make_shared<PointerType>(buffer);
	case TypeVariation::ARRAY:
		return std::make_shared<ArrayType>(buffer);
	case TypeVariation::VARIANT:
		return std::make_shared<VariantType>(buffer);
	case TypeVariation::NIL:
		return std::make_shared<NilType>(buffer);
	default:
		break;
	}

	CryImplExcept(); //TODO:
}

} // namespace CryCC::SubValue::Typedef
