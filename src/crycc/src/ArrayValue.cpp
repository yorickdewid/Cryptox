// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <CryCC/SubValue/ArrayValue.h>
#include <CryCC/SubValue/BuiltinValue.h>

namespace CryCC
{
namespace SubValue
{
namespace Valuedef
{

using namespace CryCC::SubValue::Typedef;

//FUTURE: Wrap in template method
//TODO: Encode/decode float/double fully
struct ArrayValue::PackerVisitor final : public boost::static_visitor<>
{
	Cry::ByteArray& m_buffer;

	PackerVisitor(Cry::ByteArray& buffer)
		: m_buffer{ buffer }
	{
	}

	template<typename PrimitiveType>
	void EncodeValue(const std::vector<typename PrimitiveType::storage_type>& value) const
	{
		m_buffer.SerializeAs<Cry::Byte>(PrimitiveType::specifier);
		m_buffer.SerializeAs<Cry::Word>(value.size());
		for (const auto& element : value) {
			m_buffer.SerializeAs<std::make_unsigned<PrimitiveType::storage_type>::type>(element);
		}
	}

	void operator()(const std::vector<CharType::storage_type>& value) const
	{
		EncodeValue<CharType>(value);
	}

	void operator()(const std::vector<ShortType::storage_type>& value) const
	{
		EncodeValue<ShortType>(value);
	}

	void operator()(const std::vector<IntegerType::storage_type>& value) const
	{
		EncodeValue<IntegerType>(value);
	}

	void operator()(const std::vector<LongType::storage_type>& value) const
	{
		EncodeValue<LongType>(value);
	}

	void operator()(const std::vector<UnsignedCharType::storage_type>& value) const
	{
		EncodeValue<UnsignedCharType>(value);
	}

	void operator()(const std::vector<UnsignedShortType::storage_type>& value) const
	{
		EncodeValue<UnsignedShortType>(value);
	}

	void operator()(const std::vector<UnsignedIntegerType::storage_type>& value) const
	{
		EncodeValue<UnsignedIntegerType>(value);
	}

	void operator()(const std::vector<UnsignedLongType::storage_type>& value) const
	{
		EncodeValue<UnsignedLongType>(value);
	}

	void operator()(const std::vector<FloatType::storage_type>& value) const
	{
		m_buffer.SerializeAs<Cry::Byte>(FloatType::specifier);
		m_buffer.SerializeAs<Cry::Word>(value.size());
		for (const auto& element : value) {
			m_buffer.SerializeAs<Cry::Word>(element);
		}
	}

	void operator()(const std::vector<DoubleType::storage_type>& value) const
	{
		m_buffer.SerializeAs<Cry::Byte>(DoubleType::specifier);
		m_buffer.SerializeAs<Cry::Word>(value.size());
		for (const auto& element : value) {
			m_buffer.SerializeAs<Cry::DoubleWord>(element);
		}
	}

	void operator()(const std::vector<LongDoubleType::storage_type>& value) const
	{
		m_buffer.SerializeAs<Cry::Byte>(LongDoubleType::specifier);
		m_buffer.SerializeAs<Cry::Word>(value.size());
		for (const auto& element : value) {
			m_buffer.SerializeAs<Cry::DoubleWord>(element);
		}
	}

	void operator()(const std::vector<Value2>& value) const
	{
		m_buffer.SerializeAs<Cry::Byte>(PrimitiveSpecifier::PS_RESV1);
		m_buffer.SerializeAs<Cry::Word>(value.size());
		for (const auto& element : value) {
			Value2::Serialize(element, m_buffer);
		}
	}

	template<typename PrimitiveType>
	auto DecodeValue(size_t size) const
	{
		std::vector<PrimitiveType::storage_type> value;
		for (size_t i = 0; i < size; ++i) {
			value.push_back(static_cast<PrimitiveType::storage_type>(m_buffer.Deserialize<std::make_unsigned<PrimitiveType::storage_type>::type>(Cry::ByteArray::AUTO)));
		}

		return value;
	}

	void operator()(ArrayValue::ValueVariant& variantValue) const
	{
		PrimitiveSpecifier specifier = static_cast<PrimitiveSpecifier>(m_buffer.Deserialize<Cry::Byte>(Cry::ByteArray::AUTO));
		size_t arraySize = m_buffer.Deserialize<Cry::Word>(Cry::ByteArray::AUTO);
		switch (specifier)
		{
		case CharType::specifier:
			variantValue = DecodeValue<CharType>(arraySize);
			break;
		case ShortType::specifier:
			variantValue = DecodeValue<ShortType>(arraySize);
			break;
		case IntegerType::specifier:
			variantValue = DecodeValue<IntegerType>(arraySize);
			break;
		case LongType::specifier:
			variantValue = DecodeValue<LongType>(arraySize);
			break;
		case UnsignedCharType::specifier:
			variantValue = DecodeValue<UnsignedCharType>(arraySize);
			break;
		case UnsignedShortType::specifier:
			variantValue = DecodeValue<UnsignedShortType>(arraySize);
			break;
		case UnsignedIntegerType::specifier:
			variantValue = DecodeValue<UnsignedIntegerType>(arraySize);
			break;
		case UnsignedLongType::specifier:
			variantValue = DecodeValue<UnsignedLongType>(arraySize);
			break;
		case FloatType::specifier: {
			std::vector<FloatType::storage_type> value;
			for (size_t i = 0; i < arraySize; ++i) {
				value.push_back(static_cast<FloatType::storage_type>(m_buffer.Deserialize<Cry::Word>(Cry::ByteArray::AUTO)));
			}
			variantValue = value;
			break;
		}
		case DoubleType::specifier: {
			std::vector<DoubleType::storage_type> value;
			for (size_t i = 0; i < arraySize; ++i) {
				value.push_back(static_cast<DoubleType::storage_type>(m_buffer.Deserialize<Cry::DoubleWord>(Cry::ByteArray::AUTO)));
			}
			variantValue = value;
			break;
		}
		case LongDoubleType::specifier: {
			std::vector<LongDoubleType::storage_type> value;
			for (size_t i = 0; i < arraySize; ++i) {
				value.push_back(static_cast<LongDoubleType::storage_type>(m_buffer.Deserialize<Cry::DoubleWord>(Cry::ByteArray::AUTO)));
			}
			variantValue = value;
			break;
		}
		case PrimitiveSpecifier::PS_RESV1: {
			//TODO:
			/*std::vector<Value2> value;
			for (size_t i = 0; i < arraySize; ++i) {
				Value2 temp;
				Value2::Deserialize(temp, m_buffer);
				value.emplace_back(std::move(temp));
			}
			variantValue = value;*/
			break;
		}
		default:
			CryImplExcept();
		}
	}
};

void ArrayValue::ConstructFromType()
{
	//
}

// Convert single value into data stream.
void ArrayValue::Serialize(const ArrayValue& value, Cry::ByteArray& buffer)
{
	PackerVisitor visitor{ buffer };
	value.m_value.apply_visitor(visitor);
}

// Convert data stream into single value.
void ArrayValue::Deserialize(ArrayValue& value, Cry::ByteArray& buffer)
{
	PackerVisitor visitor{ buffer };
	visitor(value.m_value);
}

// Compare to other ArrayValue.
bool ArrayValue::operator==(const ArrayValue&) const
{
	return false;
}

// Convert current value to string.
std::string ArrayValue::ToString() const
{
	return "REPLACE ME"; //TODO
}

} // namespace Valuedef
} // namespace SubValue
} // namespace CryCC
