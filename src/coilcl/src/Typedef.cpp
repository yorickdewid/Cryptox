// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "Typedef.h"

namespace CoilCl
{
namespace Typedef
{

std::vector<uint8_t> TypedefBase::TypeEnvelope() const
{
	std::vector<uint8_t> buffer;

	// Typedef base generic options
	buffer.push_back(static_cast<uint8_t>(m_isInline));
	buffer.push_back(static_cast<uint8_t>(m_storageClass));
	buffer.push_back(static_cast<uint8_t>(m_typeQualifier[0]));
	buffer.push_back(static_cast<uint8_t>(m_typeQualifier[1]));
	return buffer;
}

const std::string TypedefBase::StorageClassName() const
{
	switch (m_storageClass) {
	case StorageClassSpecifier::AUTO:		return "auto";
	case StorageClassSpecifier::STATIC:		return "static";
	case StorageClassSpecifier::EXTERN:		return "extern";
	case StorageClassSpecifier::REGISTER:	return "register";
	default:								return "";
	}
}

const std::string TypedefBase::QualifierName() const
{
	std::string result;
	for (const auto& qualifier : m_typeQualifier) {
		switch (qualifier) {
		case TypeQualifier::CONST_T:		result += "const "; break;
		case TypeQualifier::VOLATILE:	result += "volatile "; break;
		}
	}

	return result;
}

//
// BuiltinType
//

BuiltinType::BuiltinType(Specifier specifier)
	: m_specifier{ specifier }
{
	SpecifierToOptions();
}

void BuiltinType::SpecifierToOptions()
{
	switch (m_specifier) {
	case Specifier::SIGNED:
		m_typeOptions.set(IS_SIGNED);
		m_typeOptions.reset(IS_UNSIGNED);
		m_specifier = Specifier::INT;
		break;
	case Specifier::UNSIGNED:
		m_typeOptions.set(IS_UNSIGNED);
		m_typeOptions.set(IS_SIGNED);
		m_specifier = Specifier::INT;
		break;
	case Specifier::SHORT:
		m_typeOptions.set(IS_SHORT);
		m_specifier = Specifier::INT;
		break;
	case Specifier::LONG:
		m_typeOptions.set(IS_LONG);
		m_specifier = Specifier::INT;
		break;
	}
}

const std::string BuiltinType::TypeName() const
{
	auto qualifier = TypedefBase::QualifierName();

	std::string option;
	if (m_typeOptions.test(IS_UNSIGNED)) {
		option += "unsigned ";
	}
	if (m_typeOptions.test(IS_SHORT)) {
		option += "short ";
	}
	if (m_typeOptions.test(IS_LONG_LONG)) {
		option += "long long ";
	}
	else if (m_typeOptions.test(IS_LONG)) {
		option += "long ";
	}

	switch (m_specifier) {
	case Specifier::VOID_T:		qualifier += option + "void"; break;
	case Specifier::CHAR:		qualifier += option + "char"; break;
	case Specifier::LONG:		qualifier += option + "long"; break;
	case Specifier::SHORT:		qualifier += option + "short"; break;
	case Specifier::INT:		qualifier += option + "int"; break;
	case Specifier::FLOAT:		qualifier += option + "float"; break;
	case Specifier::DOUBLE:		qualifier += option + "double"; break;
	case Specifier::BOOL:		qualifier += option + "_Bool"; break;
	default:					qualifier += option + "<unknown>"; break;
	}

	return qualifier;
}

size_t BuiltinType::UnboxedSize() const
{
	switch (m_specifier) {
	case Specifier::VOID_T:		throw std::exception{};//TODO
	case Specifier::CHAR:		return sizeof(char);
	case Specifier::LONG:		return sizeof(long);
	case Specifier::SHORT:		return sizeof(short);
	case Specifier::INT:		return sizeof(int);
	case Specifier::FLOAT:		return sizeof(float);
	case Specifier::DOUBLE:		return sizeof(double);
	case Specifier::BOOL:		return sizeof(bool);
	default:					break;
	}

	throw std::exception{};//TODO
}

bool BuiltinType::Equals(TypedefBase* other) const
{
	auto self = dynamic_cast<BuiltinType*>(other);
	if (self == nullptr) {
		return false;
	}

	return m_specifier == self->m_specifier
		&& m_typeOptions == self->m_typeOptions;
}

std::vector<uint8_t> BuiltinType::TypeEnvelope() const
{
	std::vector<uint8_t> buffer = { m_c_internalType };
	buffer.push_back(static_cast<uint8_t>(m_specifier));
	const auto base = TypedefBase::TypeEnvelope();
	buffer.insert(buffer.cend(), base.cbegin(), base.cend());
	return buffer;
}

void BuiltinType::Consolidate(BaseType& type)
{
	assert(type->AllowCoalescence());

	auto otherType = std::dynamic_pointer_cast<BuiltinType>(type);
	if (otherType->Unsigned()) { m_typeOptions.set(IS_UNSIGNED); }
	if (otherType->Short()) { m_typeOptions.set(IS_SHORT); }
	if (otherType->Long()) {
		if (this->Long()) {
			m_typeOptions.set(IS_LONG_LONG);
		}
		else {
			m_typeOptions.set(IS_LONG);
		}
	}
}

//
// RecordType
//

RecordType::RecordType(const std::string& name, Specifier specifier)
	: m_name{ name }
	, m_specifier{ specifier }
{
}

//RecordType::RecordType(const std::string& name, Specifier specifier, size_t elements, BaseType type);
//	: m_name{ name }
//	, m_specifier{ specifier }
//{
//}

bool RecordType::Equals(TypedefBase* other) const
{
	auto self = dynamic_cast<RecordType*>(other);
	if (self == nullptr) {
		return false;
	}

	return m_specifier == self->m_specifier
		&& m_name == self->m_name;
}

std::vector<uint8_t> RecordType::TypeEnvelope() const
{
	std::vector<uint8_t> buffer = { m_c_internalType };
	buffer.reserve(m_name.size());
	buffer.push_back(static_cast<uint8_t>(m_name.size())); //FUTURE: Limited to 256
	buffer.insert(buffer.cend(), m_name.cbegin(), m_name.cend());
	buffer.push_back(static_cast<uint8_t>(m_specifier));
	const auto base = TypedefBase::TypeEnvelope();
	buffer.insert(buffer.cend(), base.cbegin(), base.cend());
	return buffer;
}

//
// TypedefType
//

TypedefType::TypedefType(const std::string& name, BaseType& nativeType)
	: m_name{ name }
	, m_resolveType{ std::move(nativeType) }
{
}

bool TypedefType::Equals(TypedefBase* other) const
{
	auto self = dynamic_cast<TypedefType*>(other);
	if (self == nullptr) {
		return false;
	}

	return m_resolveType == self->m_resolveType
		&& m_name == self->m_name;
}

std::vector<uint8_t> TypedefType::TypeEnvelope() const
{
	std::vector<uint8_t> buffer = { m_c_internalType };
	buffer.reserve(m_name.size());
	buffer.push_back(static_cast<uint8_t>(m_name.size())); //FUTURE: Limited to 256
	buffer.insert(buffer.cend(), m_name.cbegin(), m_name.cend());

	if (m_resolveType) {
		auto envelop = m_resolveType->TypeEnvelope();
		buffer.insert(buffer.cend(), envelop.cbegin(), envelop.cend());
	}

	const auto base = TypedefBase::TypeEnvelope();
	buffer.insert(buffer.cend(), base.cbegin(), base.cend());
	return buffer;
}

//
// VariadicType
//

std::vector<uint8_t> VariadicType::TypeEnvelope() const
{
	std::vector<uint8_t> buffer = { m_c_internalType };
	const auto base = TypedefBase::TypeEnvelope();
	buffer.insert(buffer.cend(), base.cbegin(), base.cend());
	return buffer;
}

//
// PointerType
//

std::vector<uint8_t> PointerType::TypeEnvelope() const
{
	std::vector<uint8_t> buffer = { m_c_internalType };
	const auto base = TypedefBase::TypeEnvelope();
	buffer.insert(buffer.cend(), base.cbegin(), base.cend());
	return buffer;
}

} // namespace Typedef

namespace Util
{

Typedef::BaseType MakeType(std::vector<uint8_t>&& in)
{
	using CoilCl::Typedef::TypedefBase;
	using CoilCl::Typedef::BuiltinType;
	using CoilCl::Typedef::RecordType;

	assert(in.size() > 0);
	size_t envelopeOffset = 0;
	std::shared_ptr<Typedef::TypedefBase> type;
	switch (static_cast<TypedefBase::TypeVariation>(in.at(0)))
	{
	case TypedefBase::TypeVariation::BUILTIN: {
		assert(in.size() > 1);
		const auto spec = static_cast<BuiltinType::Specifier>(in.at(1));
		envelopeOffset = 2;
		type = std::make_shared<Typedef::BuiltinType>(spec);
		break;
	}
	case TypedefBase::TypeVariation::RECORD: {
		assert(in.size() > 2);
		std::string name;
		const auto nameSize = static_cast<size_t>(in.at(1));
		name.resize(nameSize);
		CRY_MEMCPY(static_cast<void*>(&(name[0])), name.size(), &(in.at(2)), nameSize);
		//TODO: m_specifier
		//TODO: envelopeOffset = 99;
		type = std::make_shared<Typedef::RecordType>(name, RecordType::Specifier::STRUCT);
		break;
	}
	case TypedefBase::TypeVariation::TYPEDEF: {
		assert(in.size() > 2);
		std::string name;
		const auto nameSize = static_cast<size_t>(in.at(1));
		name.resize(nameSize);
		CRY_MEMCPY(static_cast<void*>(&(name[0])), name.size(), &(in.at(2)), nameSize);
		//TODO: m_resolveType
		//TODO: envelopeOffset = 99;
		std::shared_ptr<TypedefBase> q = std::make_shared<Typedef::VariadicType>();
		type = std::make_shared<Typedef::TypedefType>(name, q);
		break;
	}
	case TypedefBase::TypeVariation::VARIADIC: {
		type = std::make_shared<Typedef::VariadicType>();
		break;
	}
	case TypedefBase::TypeVariation::POINTER: {
		//TODO:
		break;
	}
	case TypedefBase::TypeVariation::INVAL:
	default:
		throw 1; //TODO: or something else
	}

	{
		assert(type);
		if (static_cast<bool>(in.at(envelopeOffset))) {
			type->SetInline();
		}

		auto spec = static_cast<TypedefBase::StorageClassSpecifier>(in.at(envelopeOffset + 1));
		type->SetStorageClass(spec);
		type->SetQualifier(static_cast<TypedefBase::TypeQualifier>(in.at(envelopeOffset + 2)));
		type->SetQualifier(static_cast<TypedefBase::TypeQualifier>(in.at(envelopeOffset + 3)));
	}

	return type;
}

} // namespace Util
} // namespace CoilCl
