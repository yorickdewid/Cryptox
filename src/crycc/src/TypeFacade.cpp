// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <CryCC/SubValue/TypeFacade.h>

namespace CryCC
{
namespace SubValue
{
namespace Typedef
{

TypeFacade::TypeFacade(const base_type& type)
	: m_type{ type }
{
}

std::string TypeFacade::TypeName() const
{
	if (!HasValue()) { return {}; }
	return m_type->TypeName() + PointerName();
}

// Convert type into data stream.
void TypeFacade::Serialize(const TypeFacade& type, Cry::ByteArray& buffer)
{
	const auto typePack = type->TypeEnvelope();
	assert(typePack.size() > 0);

	buffer.SerializeAs<Cry::Byte>(type.PointerCount());
	buffer.SerializeAs<Cry::Word>(typePack.size());
	buffer.insert(buffer.cend(), typePack.begin(), typePack.end());
}

// Convert data stream into type.
void TypeFacade::Deserialize(TypeFacade& type, Cry::ByteArray& buffer)
{
	size_t ptrCount = buffer.Deserialize<Cry::Byte>();
	size_t typePackSize = buffer.Deserialize<Cry::Word>();
	assert(typePackSize > 0);

	Cry::ByteArray tempBuffer;
	std::copy(buffer.cbegin() + buffer.Offset(), buffer.cbegin() + buffer.Offset() + typePackSize, std::back_inserter(tempBuffer));
	buffer.SetOffset(static_cast<int>(typePackSize));
	Typedef::BaseType ptr = Util::MakeType(std::move(tempBuffer));
	assert(ptr);

	// Set type facade options.
	type = TypeFacade{ ptr };
	type.SetPointer(ptrCount);
}

std::string TypeFacade::PointerName() const
{
	if (m_ptrCount == 0) { return ""; }

	return " " + std::string(m_ptrCount, '*');
}

bool TypeFacade::operator==(const TypeFacade& other) const
{
	return m_type->Equals(other.m_type.get());
}
bool TypeFacade::operator!=(const TypeFacade& other) const
{
	return !m_type->Equals(other.m_type.get());
}

} // namespace Typedef
} // namespace SubValue
} // namespace CryCC

namespace Util
{

using namespace CryCC::SubValue::Typedef;

bool IsVoid(const TypeFacade& other) noexcept
{
	const auto builtin = other.DataType<BuiltinType>();
	if (!builtin) { return false; }
	return builtin->TypeSpecifier() == BuiltinType::Specifier::VOID_T;
}

bool IsIntegral(const TypeFacade& other) noexcept
{
	const auto builtin = other.DataType<BuiltinType>();
	if (!builtin) { return false; }
	return builtin->TypeSpecifier() == BuiltinType::Specifier::INT
		|| builtin->TypeSpecifier() == BuiltinType::Specifier::INT_T;
}

bool IsFloatingPoint(const TypeFacade& other) noexcept
{
	const auto builtin = other.DataType<BuiltinType>();
	if (!builtin) { return false; }
	return builtin->TypeSpecifier() == BuiltinType::Specifier::FLOAT
		|| builtin->TypeSpecifier() == BuiltinType::Specifier::FLOAT_T;
}

bool IsArray(const TypeFacade& other) noexcept
{
	return other.IsArray();
}

bool IsEnum(const TypeFacade& /*other*/) noexcept
{
	return false;
}

bool IsStruct(const TypeFacade& other) noexcept
{
	const auto record = other.DataType<RecordType>();
	if (!record) { return false; }
	return record->TypeSpecifier() == RecordType::Specifier::STRUCT;
}

bool IsUnion(const TypeFacade& other) noexcept
{
	const auto record = other.DataType<RecordType>();
	if (!record) { return false; }
	return record->TypeSpecifier() == RecordType::Specifier::UNION;
}

bool IsClass(const TypeFacade& other) noexcept
{
	const auto record = other.DataType<RecordType>();
	if (!record) { return false; }
	return record->TypeSpecifier() == RecordType::Specifier::CLASS;
}

bool IsRecord(const TypeFacade& other) noexcept
{
	return IsStruct(other) || IsUnion(other) || IsClass(other);
}

bool IsPointer(const TypeFacade& other) noexcept
{
	return other.IsPointer();
}

bool IsInline(const TypeFacade& other) noexcept
{
	const auto builtin = other.DataType<BuiltinType>();
	if (!builtin) { return false; }
	return builtin->IsInline();
}

bool IsSensitive(const TypeFacade& other) noexcept
{
	const auto builtin = other.DataType<BuiltinType>();
	if (!builtin) { return false; }
	return builtin->IsSensitive();
}

bool IsStatic(const TypeFacade& other) noexcept
{
	return other->StorageClass() == TypedefBase::StorageClassSpecifier::STATIC;
}

bool IsExtern(const TypeFacade& other) noexcept
{
	return other->StorageClass() == TypedefBase::StorageClassSpecifier::EXTERN;
}

bool IsRegister(const TypeFacade& other) noexcept
{
	return other->StorageClass() == TypedefBase::StorageClassSpecifier::REGISTER;
}

bool IsConst(const TypeFacade& other) noexcept
{
	for (const auto& qualifier : other->TypeQualifiers()) {
		if (qualifier == TypedefBase::TypeQualifier::CONST_T) {
			return true;
		}
	}
	return false;
}

bool IsVolatile(const TypeFacade& other) noexcept
{
	for (const auto& qualifier : other->TypeQualifiers()) {
		if (qualifier == TypedefBase::TypeQualifier::VOLATILE) {
			return true;
		}
	}
	return false;
}

bool IsSigned(const TypeFacade& other) noexcept
{
	const auto builtin = other.DataType<BuiltinType>();
	if (!builtin) { return false; }
	return !builtin->Unsigned();
}

bool IsUnsigned(const TypeFacade& other) noexcept
{
	return !IsSigned(other);
}

} // namespace Util
