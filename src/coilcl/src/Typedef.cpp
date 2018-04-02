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
		case TypeQualifier::CONST:		result += "const "; break;
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
	case Specifier::VOID:		qualifier += option + "void"; break;
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
	case Specifier::VOID:		throw std::exception{};//TODO
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

void BuiltinType::Consolidate(std::shared_ptr<TypedefBase>& type)
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

bool RecordType::Equals(TypedefBase* other) const
{
	auto self = dynamic_cast<RecordType*>(other);
	if (self == nullptr) {
		return false;
	}

	return m_specifier == self->m_specifier
		&& m_name == self->m_name;
}

//
// TypedefType
//

TypedefType::TypedefType(const std::string& name, std::shared_ptr<TypedefBase>& nativeType)
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

} // namespace Typedef
} // namespace CoilCl
