// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <CryCC/SubValue/TypeHelper.h>

#include <cassert>

namespace Util
{

using namespace CryCC::SubValue::Typedef;

//
// Type creation helpers.
//

InternalBaseType MakeBuiltinType(BuiltinType::Specifier specifier)
{
	return std::make_shared<BuiltinType>(specifier);
}
InternalBaseType MakeRecordType(const std::string& name, RecordType::Specifier specifier)
{
	return std::make_shared<RecordType>(name, specifier);
}
InternalBaseType MakeTypedefType(const std::string& name, InternalBaseType& type)
{
	return std::make_shared<TypedefType>(name, type);
}
InternalBaseType MakeTypedefType(const std::string& name, TypeFacade type)
{
	return std::make_shared<TypedefType>(name, type.BaseType());
}
InternalBaseType MakeVariadicType()
{
	return std::make_shared<VariadicType>();
}
InternalBaseType MakeArrayType(size_t elements, InternalBaseType& type)
{
	return std::make_shared<ArrayType>(elements, type);
}
InternalBaseType MakeVariantType()
{
	//TODO:
	return nullptr;
}
InternalBaseType MakeNilType()
{
	return std::make_shared<NilType>();
}
InternalBaseType MakePointerType(InternalBaseType& type)
{
	return std::make_shared<PointerType>(type);
}
InternalBaseType MakePointerType(TypeFacade type)
{
	return std::make_shared<PointerType>(type.BaseType());
}

template<typename CastType>
auto InternalTypeCast(const InternalBaseType& type) noexcept
{
	return std::dynamic_pointer_cast<CastType>(type);
}

//
// Type query helpers.
//

namespace
{

template<TypeVariation Variant>
bool CheckVariant(const InternalBaseType& type) noexcept
{
	return type->TypeId() == Variant;
}

BuiltinType::Specifier BuiltinTypeSpecifier(const InternalBaseType& type)
{
	if (!CheckVariant<TypeVariation::BUILTIN>(type)) { throw std::exception{}; }
	const auto builtin = InternalTypeCast<BuiltinType>(type);
	assert(builtin);
	return builtin->TypeSpecifier();
}

RecordType::Specifier RecordTypeSpecifier(const InternalBaseType& type)
{
	if (!CheckVariant<TypeVariation::RECORD>(type)) { throw std::exception{}; }
	const auto record = InternalTypeCast<RecordType>(type);
	assert(record);
	return record->TypeSpecifier();
}

}

bool IsVoid(const InternalBaseType& type) noexcept
{
	return BuiltinTypeSpecifier(type) == BuiltinType::Specifier::VOID_T;
}

bool IsIntegral(const InternalBaseType& type) noexcept
{
	try {
		switch (BuiltinTypeSpecifier(type))
		{
		case BuiltinType::Specifier::BOOL_T:
		case BuiltinType::Specifier::CHAR_T:
		case BuiltinType::Specifier::SIGNED_CHAR_T:
		case BuiltinType::Specifier::UNSIGNED_CHAR_T:
		case BuiltinType::Specifier::SHORT_T:
		case BuiltinType::Specifier::UNSIGNED_SHORT_T:
		case BuiltinType::Specifier::INT_T:
		case BuiltinType::Specifier::UNSIGNED_INT_T:
		case BuiltinType::Specifier::LONG_T:
		case BuiltinType::Specifier::UNSIGNED_LONG_T:
			return true;
		}

		return false;
	}
	catch (...) { return false; }
}

bool IsFloatingPoint(const InternalBaseType& type) noexcept
{
	try {
		return BuiltinTypeSpecifier(type) == BuiltinType::Specifier::FLOAT_T;
	}
	catch (...) { return false; }
}

bool IsBool(const InternalBaseType& type) noexcept
{
	try {
		return BuiltinTypeSpecifier(type) == BuiltinType::Specifier::BOOL_T;
	}
	catch (...) { return false; }
}

bool IsArray(const InternalBaseType& type) noexcept
{
	return CheckVariant<TypeVariation::ARRAY>(type);
}

bool IsEnum(const InternalBaseType& /*type*/) noexcept
{
	//TODO:
	return false;
}

bool IsStruct(const InternalBaseType& type) noexcept
{
	try {
		return RecordTypeSpecifier(type) == RecordType::Specifier::STRUCT;
	}
	catch (...) { return false; }
}

bool IsUnion(const InternalBaseType& type) noexcept
{
	try {
		return RecordTypeSpecifier(type) == RecordType::Specifier::UNION;
	}
	catch (...) { return false; }
}

bool IsClass(const InternalBaseType& type) noexcept
{
	try {
		return RecordTypeSpecifier(type) == RecordType::Specifier::CLASS;
	}
	catch (...) { return false; }
}

bool IsRecord(const InternalBaseType& type) noexcept
{
	return IsStruct(type) || IsUnion(type) || IsClass(type);
}

bool IsPointer(const InternalBaseType& type) noexcept
{
	return CheckVariant<TypeVariation::POINTER>(type);
}

bool IsInline(const InternalBaseType& type) noexcept
{
	return type->IsInline();
}

bool IsSensitive(const InternalBaseType& type) noexcept
{
	return type->IsSensitive();
}

bool IsAuto(const InternalBaseType& type) noexcept
{
	return type->StorageClass() == AbstractType::StorageClassSpecifier::AUTO_T;
}

bool IsStatic(const InternalBaseType& type) noexcept
{
	return type->StorageClass() == AbstractType::StorageClassSpecifier::STATIC_T;
}

bool IsExtern(const InternalBaseType& type) noexcept
{
	return type->StorageClass() == AbstractType::StorageClassSpecifier::EXTERN_T;
}

bool IsRegister(const InternalBaseType& type) noexcept
{
	return type->StorageClass() == AbstractType::StorageClassSpecifier::REGISTER_T;
}

bool IsConst(const InternalBaseType& type) noexcept
{
	for (const auto& qualifier : type->TypeQualifiers()) {
		if (qualifier == AbstractType::TypeQualifier::CONST_T) {
			return true;
		}
	}
	return false;
}

bool IsVolatile(const InternalBaseType& type) noexcept
{
	for (const auto& qualifier : type->TypeQualifiers()) {
		if (qualifier == AbstractType::TypeQualifier::VOLATILE_T) {
			return true;
		}
	}
	return false;
}

bool IsSigned(const InternalBaseType& type) noexcept
{
	if (!CheckVariant<TypeVariation::BUILTIN>(type)) { return false; }
	const auto builtin = InternalTypeCast<BuiltinType>(type);
	assert(builtin);
	return !builtin->Unsigned();
}

bool IsUnsigned(const InternalBaseType& type) noexcept
{
	return !IsSigned(type);
}

} // namespace Util
