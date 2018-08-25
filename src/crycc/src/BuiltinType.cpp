// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <CryCC/SubValue/Typedef.h>

#define BITS_PER_BYTE 8

namespace CryCC
{
namespace SubValue
{
namespace Typedef
{

template<typename TypeAlias, typename Type>
struct PrimitiveType
{
    using type = Type;
	using alias = typename std::remove_reference<typename std::remove_cv<TypeAlias>::type>::type;
	static const bool is_unsigned = std::is_unsigned<TypeAlias>::value;
    static const int bit_count = sizeof(Type) * BITS_PER_BYTE;
};

using CharType = PrimitiveType<char, int8_t>;
using SignedCharType = PrimitiveType<signed char, int8_t>;
using UnsignedCharType = PrimitiveType<unsigned char, uint8_t>;
using ShortType = PrimitiveType<short, int16_t>;
using UnsignedShortType = PrimitiveType<unsigned short, uint16_t>;
using IntegerType = PrimitiveType<int, int32_t>;
using UnsignedIntegerType = PrimitiveType<unsigned int, uint32_t>;
using LongType = PrimitiveType<long, int64_t>;
using UnsignedLongType = PrimitiveType<unsigned long, uint64_t>;
using FloatType = PrimitiveType<float, int32_t>;
using DoubleType = PrimitiveType<double, int64_t>;
using LongDoubleType = PrimitiveType<long double, int64_t>;
using UnsignedLongDoubleType = PrimitiveType<unsigned long double, uint64_t>;

BuiltinType::BuiltinType(Specifier specifier)
	: m_specifier{ specifier }
{
    // Convert specifiers into type options.
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

BuiltinType::buffer_type BuiltinType::TypeEnvelope() const
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

} // namespace Typedef
} // namespace SubValue
} // namespace CryCC
