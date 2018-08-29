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

using namespace CryCC::SubValue::Typedef;

//FUTURE: Wrap in template method
struct BuiltinValue::PackerVisitor final : public boost::static_visitor<>
{
	Cry::ByteArray& m_buffer;

	PackerVisitor(Cry::ByteArray& buffer)
		: m_buffer{ buffer }
	{
	}
	
	template<typename PrimitiveType>
	void EncodeValue(typename PrimitiveType::storage_type value) const
	{
		m_buffer.SerializeAs<Cry::Byte>(PrimitiveType::specifier);
		m_buffer.SerializeAs<std::make_unsigned<PrimitiveType::storage_type>::type>(value);
	}

	void operator()(typename CharType::storage_type value) const
	{
		EncodeValue<CharType>(value);
	}
	void operator()(typename ShortType::storage_type value) const
	{
		EncodeValue<ShortType>(value);
	}
	void operator()(typename IntegerType::storage_type value) const
	{
		EncodeValue<IntegerType>(value);
	}
	void operator()(typename LongType::storage_type value) const
	{
		EncodeValue<LongType>(value);
	}
	void operator()(typename UnsignedCharType::storage_type value) const
	{
		EncodeValue<UnsignedCharType>(value);
	}
	void operator()(typename UnsignedShortType::storage_type value) const
	{
		EncodeValue<UnsignedShortType>(value);
	}
	void operator()(typename UnsignedIntegerType::storage_type value) const
	{
		EncodeValue<UnsignedIntegerType>(value);
	}
	void operator()(typename UnsignedLongType::storage_type value) const
	{
		EncodeValue<UnsignedLongType>(value);
	}
	void operator()(typename FloatType::storage_type value) const
	{
		m_buffer.SerializeAs<Cry::Byte>(FloatType::specifier);
		m_buffer.SerializeAs<Cry::Word>(value);
	}
	void operator()(typename DoubleType::storage_type value) const
	{
		m_buffer.SerializeAs<Cry::Byte>(DoubleType::specifier);
		m_buffer.SerializeAs<Cry::DoubleWord>(value);
	}
	void operator()(typename LongDoubleType::storage_type value) const
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
		switch (static_cast<PrimitiveSpecifier>(m_buffer.Deserialize<Cry::Byte>(Cry::ByteArray::AUTO)))
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

// Convert single value into data stream.
void BuiltinValue::Serialize(const BuiltinValue& value, buffer_type& buffer)
{
	PackerVisitor valueVisitor{ buffer };
	value.m_value.apply_visitor(valueVisitor);
}

// Convert data stream into single value.
void BuiltinValue::Deserialize(BuiltinValue& value, buffer_type& buffer)
{
	PackerVisitor valueVisitor{ buffer };
	valueVisitor(value.m_value);
}

// Compare to other BuiltinValue.
bool BuiltinValue::operator==(const BuiltinValue&) const
{
	return false;
}

// Convert current value to string.
std::string BuiltinValue::ToString() const
{
	return "REPLACE ME"; //TODO
}

} // namespace Valuedef
} // namespace SubValue
} // namespace CryCC
