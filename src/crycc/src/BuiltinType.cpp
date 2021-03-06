// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <CryCC/SubValue/BuiltinType.h>

#include <cassert>

namespace CryCC::SubValue::Typedef
{

BuiltinType::buffer_type& operator<<(BuiltinType::buffer_type& os, const BuiltinType::Specifier& specifier)
{
	os << static_cast<Cry::Byte>(specifier);
	return os;
}

BuiltinType::buffer_type& operator>>(BuiltinType::buffer_type& os, BuiltinType::Specifier& specifier)
{
	specifier = BuiltinType::Specifier::VOID_T;
	os >> reinterpret_cast<Cry::Byte&>(specifier);
	return os;
}

BuiltinType::buffer_type& operator>>(BuiltinType::buffer_type& os, std::bitset<8>& options)
{
	unsigned long _options{ 0 };
	os >> _options;
	options = _options;
	return os;
}

BuiltinType::BuiltinType(Specifier specifier)
	: m_specifier{ specifier }
{
	SpecifierToOptions();
}

BuiltinType::BuiltinType(buffer_type& buffer)
{
	Unpack(buffer);
}

// Extract specifier properties.
void BuiltinType::SpecifierToOptions()
{
	switch (m_specifier) {
	case Specifier::CHAR_T:
	case Specifier::SIGNED_CHAR_T:
	case Specifier::SHORT_T:
	case Specifier::INT_T:
	case Specifier::LONG_T:
		m_typeOptions.set(IS_SIGNED);
		m_typeOptions.reset(IS_UNSIGNED);
		break;
	case Specifier::UNSIGNED_CHAR_T:
	case Specifier::UNSIGNED_SHORT_T:
	case Specifier::UNSIGNED_INT_T:
	case Specifier::UNSIGNED_LONG_T:
	case Specifier::UNSIGNED_LONG_DOUBLE_T:
		m_typeOptions.set(IS_UNSIGNED);
		m_typeOptions.reset(IS_SIGNED);
		break;
	}
}

void BuiltinType::Pack(buffer_type& buffer) const
{
	AbstractType::Pack(buffer);

	buffer << m_specifier;
	buffer << m_typeOptions.to_ulong();
}

void BuiltinType::Unpack(buffer_type& buffer)
{
	AbstractType::Unpack(buffer);

	buffer >> m_specifier;
	buffer >> m_typeOptions;
}

const std::string BuiltinType::ToString() const
{
	auto qualifier = AbstractType::QualifierName();

	switch (m_specifier) {
	case Specifier::VOID_T:
		qualifier += "void";
		break;
	case Specifier::BOOL_T:
		qualifier += "bool";
		break;
	case Specifier::CHAR_T:
		qualifier += "char";
		break;
	case Specifier::SIGNED_CHAR_T:
		qualifier += "signed char";
		break;
	case Specifier::UNSIGNED_CHAR_T:
		qualifier += "unsigned char";
		break;
	case Specifier::SHORT_T:
		qualifier += "short";
		break;
	case Specifier::UNSIGNED_SHORT_T:
		qualifier += "unsigned short";
		break;
	case Specifier::INT_T:
		qualifier += "int";
		break;
	case Specifier::UNSIGNED_INT_T:
		qualifier += "unsigned int";
		break;
	case Specifier::LONG_T:
		qualifier += "long";
		break;
	case Specifier::UNSIGNED_LONG_T:
		qualifier += "unsigned long";
		break;
	case Specifier::FLOAT_T:
		qualifier += "float";
		break;
	case Specifier::DOUBLE_T:
		qualifier += "double";
		break;
	case Specifier::LONG_DOUBLE_T:
		qualifier += "long double";
		break;
	case Specifier::UNSIGNED_LONG_DOUBLE_T:
		qualifier += "unsigned long double";
		break;
	default:
		qualifier += "<unknown>";
		break;
	}

	return qualifier;
}

size_t BuiltinType::UnboxedSize() const
{
	//TODO: use Cry::Types
	switch (m_specifier) {
	case Specifier::VOID_T:	return 0;
	case Specifier::BOOL_T: return sizeof(bool);
	case Specifier::CHAR_T: return sizeof(char);
	case Specifier::SIGNED_CHAR_T: return sizeof(signed char);
	case Specifier::UNSIGNED_CHAR_T: return sizeof(unsigned char);
	case Specifier::SHORT_T: return sizeof(short);
	case Specifier::UNSIGNED_SHORT_T: return sizeof(unsigned short);
	case Specifier::INT_T: return sizeof(int);
	case Specifier::UNSIGNED_INT_T: return sizeof(unsigned int);
	case Specifier::LONG_T: return sizeof(long);
	case Specifier::UNSIGNED_LONG_T: return sizeof(unsigned long);
	case Specifier::FLOAT_T: return sizeof(float);
	case Specifier::DOUBLE_T: return sizeof(double);
	case Specifier::LONG_DOUBLE_T: return sizeof(long double);
		//case Specifier::UNSIGNED_LONG_DOUBLE_T:		return sizeof(unsigned long double);
	}

	CryImplExcept(); //TODO:
}

bool BuiltinType::Equals(const InternalBaseType& other) const
{
	if (TypeId() != other->TypeId()) { return false; }
	return operator==(dynamic_cast<const BuiltinType&>(*other));
}

void BuiltinType::Consolidate(InternalBaseType& type)
{
	assert(type->AllowCoalescence());

	auto otherType = std::dynamic_pointer_cast<BuiltinType>(type);
	if (otherType->Unsigned()) {
		m_typeOptions.set(IS_UNSIGNED);
	}
	if (otherType->Short()) {
		m_typeOptions.set(IS_SHORT);
	}
	if (otherType->Long()) {
		if (this->Long()) {
			m_typeOptions.set(IS_LONG_LONG);
		}
		else {
			m_typeOptions.set(IS_LONG);
		}
	}
}

bool BuiltinType::operator==(const BuiltinType& other) const
{
	return AbstractType::operator==(other)
		&& m_specifier == other.m_specifier
		&& m_typeOptions == other.m_typeOptions;
}

bool BuiltinType::operator!=(const BuiltinType& other) const
{
	return !operator==(other);
}

} // namespace CryCC::SubValue::Typedef
