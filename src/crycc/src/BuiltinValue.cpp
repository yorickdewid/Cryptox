// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <CryCC/SubValue/BuiltinValue.h>

namespace CryCC
{
namespace SubValue
{
namespace Valuedef
{

using namespace Cry;

//FUTURE: Wrap in template method
//TODO: Encode/decode float/double fully
struct BuiltinValue::PackerVisitor final : public boost::static_visitor<>
{
	Cry::ByteArray& m_buffer;

	PackerVisitor(Cry::ByteArray& buffer)
		: m_buffer{ buffer }
	{
	}

	template<typename PrimitiveType>
	void EncodeValue(const typename PrimitiveType::storage_type& value) const
	{
		m_buffer.SerializeAs<Cry::Byte>(PrimitiveType::specifier);
		m_buffer.SerializeAs<std::make_unsigned<PrimitiveType::storage_type>::type>(value);
	}

	void operator()(const typename CharType::storage_type& value) const
	{
		EncodeValue<CharType>(value);
	}

	void operator()(const typename ShortType::storage_type& value) const
	{
		EncodeValue<ShortType>(value);
	}

	void operator()(const typename IntegerType::storage_type& value) const
	{
		EncodeValue<IntegerType>(value);
	}

	void operator()(const typename LongType::storage_type& value) const
	{
		EncodeValue<LongType>(value);
	}

	void operator()(const typename UnsignedCharType::storage_type& value) const
	{
		EncodeValue<UnsignedCharType>(value);
	}

	void operator()(const typename UnsignedShortType::storage_type& value) const
	{
		EncodeValue<UnsignedShortType>(value);
	}

	void operator()(const typename UnsignedIntegerType::storage_type& value) const
	{
		EncodeValue<UnsignedIntegerType>(value);
	}

	void operator()(const typename UnsignedLongType::storage_type& value) const
	{
		EncodeValue<UnsignedLongType>(value);
	}

	void operator()(const typename FloatType::storage_type& value) const
	{
		m_buffer.SerializeAs<Cry::Byte>(FloatType::specifier);
		m_buffer.SerializeAs<Cry::Word>(value);
	}

	void operator()(const typename DoubleType::storage_type& value) const
	{
		m_buffer.SerializeAs<Cry::Byte>(DoubleType::specifier);
		m_buffer.SerializeAs<Cry::DoubleWord>(value);
	}

	void operator()(const typename LongDoubleType::storage_type& value) const
	{
		m_buffer.SerializeAs<Cry::Byte>(LongDoubleType::specifier);
		m_buffer.SerializeAs<Cry::DoubleWord>(value);
	}

	template<typename PrimitiveType>
	auto DecodeValue() const
	{
		return static_cast<PrimitiveType::storage_type>(m_buffer.Deserialize<std::make_unsigned<PrimitiveType::storage_type>::type>(Cry::ByteArray::AUTO));
	}

	void operator()(BuiltinValue::ValueVariant& variantValue) const
	{
		PrimitiveSpecifier specifier = static_cast<PrimitiveSpecifier>(m_buffer.Deserialize<Cry::Byte>(Cry::ByteArray::AUTO));
		switch (specifier)
		{
		case CharType::specifier:
			variantValue = DecodeValue<CharType>();
			break;
		case ShortType::specifier:
			variantValue = DecodeValue<ShortType>();
			break;
		case IntegerType::specifier:
			variantValue = DecodeValue<IntegerType>();
			break;
		case LongType::specifier:
			variantValue = DecodeValue<LongType>();
			break;
		case UnsignedCharType::specifier:
			variantValue = DecodeValue<UnsignedCharType>();
			break;
		case UnsignedShortType::specifier:
			variantValue = DecodeValue<UnsignedShortType>();
			break;
		case UnsignedIntegerType::specifier:
			variantValue = DecodeValue<UnsignedIntegerType>();
			break;
		case UnsignedLongType::specifier:
			variantValue = DecodeValue<UnsignedLongType>();
			break;
		case FloatType::specifier:
			variantValue = static_cast<FloatType::storage_type>(m_buffer.Deserialize<Cry::Word>(Cry::ByteArray::AUTO));
			break;
		case DoubleType::specifier:
			variantValue = static_cast<DoubleType::storage_type>(m_buffer.Deserialize<Cry::DoubleWord>(Cry::ByteArray::AUTO));
			break;
		case LongDoubleType::specifier:
			variantValue = static_cast<LongDoubleType::storage_type>(m_buffer.Deserialize<Cry::DoubleWord>(Cry::ByteArray::AUTO));
			break;
		default:
			CryImplExcept();
		}
	}
};

void BuiltinValue::Serialize(const BuiltinValue& value, buffer_type& buffer)
{
	PackerVisitor valueVisitor{ buffer };
	value.m_value.apply_visitor(valueVisitor);
}

void BuiltinValue::Deserialize(BuiltinValue& value, buffer_type& buffer)
{
	PackerVisitor valueVisitor{ buffer };
	valueVisitor(value.m_value);
}

bool BuiltinValue::operator==(const BuiltinValue&) const
{
	return false;
}

struct StringifyVisitor final : public boost::static_visitor<>
{
	std::string stringValue;

	template<typename Type>
	void operator()(const Type& value)
	{
		stringValue = std::to_string(value);
	}
};

std::string BuiltinValue::ToString() const
{
	StringifyVisitor valueVisitor;
	m_value.apply_visitor(valueVisitor);
	return valueVisitor.stringValue;
}

template<template<typename> typename BinaryOperation, auto Incr = 1>
struct AlternateVisitor final : public boost::static_visitor<>
{
	template<typename Type>
	void operator()(Type& value)
	{
		value = std::invoke(BinaryOperation<Type>(), value, Type{ Incr });
	}
};

BuiltinValue& BuiltinValue::operator++()
{
	AlternateVisitor<std::plus> valueVisitor;
	m_value.apply_visitor(valueVisitor);
	return (*this);
}

BuiltinValue& BuiltinValue::operator--()
{
	AlternateVisitor<std::minus> valueVisitor;
	m_value.apply_visitor(valueVisitor);
	return (*this);
}

BuiltinValue BuiltinValue::operator++(int)
{
	BuiltinValue tmp = std::as_const(*this);
	AlternateVisitor<std::plus> valueVisitor;
	m_value.apply_visitor(valueVisitor);
	return tmp;
}

BuiltinValue BuiltinValue::operator--(int)
{
	BuiltinValue tmp = std::as_const(*this);
	AlternateVisitor<std::minus> valueVisitor;
	m_value.apply_visitor(valueVisitor);
	return tmp;
}

template<typename IntermediateType>
struct CastVisitor final : public boost::static_visitor<>
{
	IntermediateType commonValue = 0;

	template<typename Type>
	void operator()(const Type& value)
	{
		commonValue = static_cast<IntermediateType>(value);
	}
};

BuiltinValue operator+(const BuiltinValue& lhs, const BuiltinValue& rhs)
{
	CastVisitor<long int> valueVisitor;
	lhs.m_value.apply_visitor(valueVisitor);
	CastVisitor<long int> valueVisitor2;
	rhs.m_value.apply_visitor(valueVisitor2);
	return valueVisitor.commonValue + valueVisitor2.commonValue;
}

BuiltinValue operator-(const BuiltinValue& lhs, const BuiltinValue& rhs)
{
	CastVisitor<long int> valueVisitor;
	lhs.m_value.apply_visitor(valueVisitor);
	CastVisitor<long int> valueVisitor2;
	rhs.m_value.apply_visitor(valueVisitor2);
	return valueVisitor.commonValue - valueVisitor2.commonValue;
}

BuiltinValue operator*(const BuiltinValue& lhs, const BuiltinValue& rhs)
{
	CastVisitor<long int> valueVisitor;
	lhs.m_value.apply_visitor(valueVisitor);
	CastVisitor<long int> valueVisitor2;
	rhs.m_value.apply_visitor(valueVisitor2);
	return valueVisitor.commonValue * valueVisitor2.commonValue;
}

BuiltinValue operator/(const BuiltinValue& lhs, const BuiltinValue& rhs)
{
	CastVisitor<long int> valueVisitor;
	lhs.m_value.apply_visitor(valueVisitor);
	CastVisitor<long int> valueVisitor2;
	rhs.m_value.apply_visitor(valueVisitor2);
	return valueVisitor.commonValue / valueVisitor2.commonValue;
}

BuiltinValue operator%(const BuiltinValue& lhs, const BuiltinValue& rhs)
{
	CastVisitor<long int> valueVisitor;
	lhs.m_value.apply_visitor(valueVisitor);
	CastVisitor<long int> valueVisitor2;
	rhs.m_value.apply_visitor(valueVisitor2);
	return valueVisitor.commonValue % valueVisitor2.commonValue;
}

} // namespace Valuedef
} // namespace SubValue
} // namespace CryCC
