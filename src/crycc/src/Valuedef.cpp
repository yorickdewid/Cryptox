// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <CryCC/SubValue/Valuedef.h>
#include <CryCC/SubValue/ValueHelper.h> // << TODO: Maybe remove?

#include <Cry/Cry.h>
#include <Cry/ByteOrder.h>

#define VALUE_MAGIC 0x7a

namespace CryCC
{
namespace SubValue
{
namespace Valuedef
{

// Value declaration without initialization.
Value::Value(Typedef::TypeFacade typeBase)
	: m_internalType{ typeBase }
{
}

// Value declaration and initialization.
Value::Value(Typedef::TypeFacade typeBase, ValueVariantSingle&& value)
	: m_internalType{ typeBase }
	, m_value{ ValueSelect{ std::move(value) } }
{
}

// Value declaration and initialization. Pass elements to value type in order for 
// the array size to match with the value storage.
Value::Value(Typedef::TypeFacade typeBase, ValueVariantMulti&& value, size_t elements)
	: m_internalType{ typeBase }
	, m_value{ ValueSelect{ std::move(value) } }
{
	m_internalType.SetArraySize(elements);
}

// Value declaration and initialization. Convert the record value definition
// to the record type.
Value::Value(Typedef::TypeFacade typeBase, RecordValue&& value)
	: m_internalType{ typeBase }
	, m_value{ ValueSelect{ std::move(value) } }
{
	auto *record = dynamic_cast<Typedef::RecordType *>(m_internalType.operator->());
	assert(record);

	//FUTURE: Improve this structure and copy the record with transform to recordtype.
	const auto recordValue = this->As<RecordValue>();
	for (size_t i = 0; i < recordValue.Size(); ++i) {
		const auto fieldTypeFacade = std::make_shared<Typedef::TypeFacade>(recordValue.At(i)->Type());
		record->AddField(recordValue.FieldName(i), fieldTypeFacade);
	}
}

// Value declaration and initialization.
Value::Value(Typedef::TypeFacade typeBase, Value&& value)
	: m_internalType{ typeBase }
	, m_value{ ValueSelect{ std::move(value) } }
{
}

Value::ValueSelect::ValueSelect(ValueVariantSingle value)
	: singleValue{ value }
{
}

Value::ValueSelect::ValueSelect(ValueVariantMulti value)
	: multiValue{ value }
{
}

Value::ValueSelect::ValueSelect(RecordValue&& value)
	: recordValue{ std::move(value) }
{
}

Value::ValueSelect::ValueSelect(Value&& value)
	: referenceValue{ std::make_shared<Value>(value) }
{
}

// Make the value uninitialized by unsetting all current values.
void Value::ValueSelect::Clear()
{
	singleValue = boost::none;
	multiValue = boost::none;
	recordValue = boost::none;
	referenceValue.reset();
	assert(Empty());
}

// Clear current value and copy value.
Value::ValueSelect& Value::ValueSelect::operator=(const ValueSelect& other)
{
	Clear();
	if (other.singleValue) {
		singleValue = other.singleValue;
	}
	else if (other.multiValue) {
		multiValue = other.multiValue;
	}
	else if (other.recordValue) {
		recordValue.emplace(other.recordValue.get());
	}
	else if (other.referenceValue) {
		referenceValue = other.referenceValue;
	}

	return (*this);
}

// Clear current value and copy value, then clear other value.
Value::ValueSelect& Value::ValueSelect::operator=(ValueSelect&& other)
{
	Clear();
	if (other.singleValue) {
		singleValue = std::move(other.singleValue);
	}
	else if (other.multiValue) {
		multiValue = std::move(other.multiValue);
	}
	else if (other.recordValue) {
		recordValue.emplace(other.recordValue.get());
	}
	else if (other.referenceValue) {
		referenceValue = std::move(other.referenceValue);
	}

	other.Clear();
	return (*this);
}

namespace
{

struct ValuePacker : public boost::static_visitor<>
{
	enum NativeValueType : Cry::Byte
	{
		INT = 100, CHAR, FLOAT, DOUBLE, BOOL,
		INTA, CHARA, FLOATA, DOUBLEA, BOOLA,
	};

	ValuePacker(Cry::ByteArray& buffer)
		: m_buffer{ buffer }
	{
	}

protected:
	Cry::ByteArray& m_buffer;
};

struct ValuePackerSingular final : public ValuePacker
{
	ValuePackerSingular(Cry::ByteArray& buffer)
		: ValuePacker{ buffer }
	{
	}

	void operator()(int v) const
	{
		m_buffer.SerializeAs<Cry::Byte>(NativeValueType::INT);
		m_buffer.SerializeAs<Cry::Word>(v);
	}

	void operator()(char v) const
	{
		m_buffer.SerializeAs<Cry::Byte>(NativeValueType::CHAR);
		m_buffer.SerializeAs<Cry::Byte>(v);
	}

	void operator()(float v) const
	{
		m_buffer.SerializeAs<Cry::Byte>(NativeValueType::FLOAT);
		m_buffer.SerializeAs<Cry::Word>(v);
	}

	void operator()(double v) const
	{
		m_buffer.SerializeAs<Cry::Byte>(NativeValueType::DOUBLE);
		m_buffer.SerializeAs<Cry::DoubleWord>(v);
	}

	void operator()(bool v) const
	{
		m_buffer.SerializeAs<Cry::Byte>(NativeValueType::BOOL);
		m_buffer.SerializeAs<Cry::Byte>(v);
	}

	Value::ValueVariantSingle Load() const
	{
		switch (static_cast<NativeValueType>(m_buffer.Deserialize<Cry::Byte>(Cry::ByteArray::AUTO)))
		{
		case NativeValueType::INT:
			return static_cast<int>(m_buffer.Deserialize<Cry::Word>(Cry::ByteArray::AUTO));
		case NativeValueType::CHAR:
			return static_cast<char>(m_buffer.Deserialize<Cry::Byte>(Cry::ByteArray::AUTO));
		case NativeValueType::FLOAT:
			return static_cast<float>(m_buffer.Deserialize<Cry::Word>(Cry::ByteArray::AUTO));
		case NativeValueType::DOUBLE:
			return static_cast<double>(m_buffer.Deserialize<Cry::DoubleWord>(Cry::ByteArray::AUTO));
		case NativeValueType::BOOL: {
			return static_cast<bool>(m_buffer.Deserialize<Cry::Byte>(Cry::ByteArray::AUTO));
		}
		default:
			CryImplExcept();
		}
	}
};

struct ValuePackerMulti final : public ValuePacker
{
	ValuePackerMulti(Cry::ByteArray& buffer)
		: ValuePacker{ buffer }
	{
	}

	void operator()(const std::vector<int>& v) const
	{
		m_buffer.SerializeAs<Cry::Byte>(NativeValueType::INTA);
		m_buffer.SerializeAs<Cry::Word>(v.size());
		for (const auto element : v) {
			m_buffer.SerializeAs<Cry::Word>(element);
		}
	}

	void operator()(const std::vector<char>& v) const
	{
		m_buffer.SerializeAs<Cry::Byte>(NativeValueType::CHARA);
		m_buffer.SerializeAs<Cry::Word>(v.size());
		for (const auto element : v) {
			m_buffer.SerializeAs<Cry::Byte>(element);
		}
	}

	void operator()(const std::vector<float>& v) const
	{
		m_buffer.SerializeAs<Cry::Byte>(NativeValueType::FLOATA);
		m_buffer.SerializeAs<Cry::Word>(v.size());
		for (const auto element : v) {
			m_buffer.SerializeAs<Cry::Word>(element);
		}
	}

	void operator()(const std::vector<double>& v) const
	{
		m_buffer.SerializeAs<Cry::Byte>(NativeValueType::DOUBLEA);
		m_buffer.SerializeAs<Cry::Word>(v.size());
		for (const auto element : v) {
			m_buffer.SerializeAs<Cry::DoubleWord>(element);
		}
	}

	void operator()(const std::vector<bool>& v) const
	{
		m_buffer.SerializeAs<Cry::Byte>(NativeValueType::BOOLA);
		m_buffer.SerializeAs<Cry::Word>(v.size());
		for (const auto element : v) {
			m_buffer.SerializeAs<Cry::Byte>(element);
		}
	}

	Value::ValueVariantMulti Load() const
	{
		NativeValueType select = static_cast<NativeValueType>(m_buffer.Deserialize<Cry::Byte>(Cry::ByteArray::AUTO));
		size_t arraySize = m_buffer.Deserialize<Cry::Word>(Cry::ByteArray::AUTO);
		switch (select)
		{
		case NativeValueType::INTA: {
			std::vector<int> v;
			for (size_t i = 0; i < arraySize; ++i) {
				v.push_back(static_cast<int>(m_buffer.Deserialize<Cry::Word>(Cry::ByteArray::AUTO)));
			}
			return v;
		}
		case NativeValueType::CHARA: {
			std::vector<char> v;
			for (size_t i = 0; i < arraySize; ++i) {
				v.push_back(static_cast<char>(m_buffer.Deserialize<Cry::Byte>(Cry::ByteArray::AUTO)));
			}
			return v;
		}
		case NativeValueType::FLOATA: {
			std::vector<float> v;
			for (size_t i = 0; i < arraySize; ++i) {
				v.push_back(static_cast<float>(m_buffer.Deserialize<Cry::Word>(Cry::ByteArray::AUTO)));
			}
			return v;
		}
		case NativeValueType::DOUBLEA: {
			std::vector<double> v;
			for (size_t i = 0; i < arraySize; ++i) {
				v.push_back(static_cast<double>(m_buffer.Deserialize<Cry::DoubleWord>(Cry::ByteArray::AUTO)));
			}
			return v;
		}
		case NativeValueType::BOOLA: {
			std::vector<bool> v;
			for (size_t i = 0; i < arraySize; ++i) {
				v.push_back(static_cast<bool>(m_buffer.Deserialize<Cry::Byte>(Cry::ByteArray::AUTO)));
			}
			return v;
		}
		default:
			CryImplExcept();
		}
	}
};

void PointerPacker(const std::shared_ptr<Value>& pointer, Cry::ByteArray& buffer)
{
	buffer.Serialize(pointer ? Cry::Byte{ 1 } : Cry::Byte{ 0 });
	if (pointer) {
		Value::Serialize((*pointer), buffer);
	}
}

void PointerUnpacker(Value& pointer, Cry::ByteArray& buffer)
{
	if (buffer.Deserialize<Cry::Byte>()) {
		Value::Deserialize(pointer, buffer);
	}
}

struct ConvertToStringSingular final : public boost::static_visitor<>
{
	std::string output;

	template<typename NativeType>
	void operator()(NativeType& value)
	{
		output = boost::lexical_cast<std::string>(value);
	}
};

struct ConvertToStringMulti final : public boost::static_visitor<>
{
	std::string output;

	template<typename NativeType>
	void operator()(NativeType& value)
	{
		//TODO: convert list to string
		CRY_UNUSED(value);
		output = "list";// boost::lexical_cast<std::string>(value);
	}
};

} // namespace

enum ValueSelectType
{
	SINGLE = 10,
	MULTI,
	RECORD,
	REFERENCE,
};

// Convert value into byte stream.
void Value::ValueSelect::Pack(const ValueSelect& value, Cry::ByteArray& buffer)
{
	if (value.singleValue) {
		buffer.SerializeAs<Cry::Byte>(ValueSelectType::SINGLE);
		ValuePackerSingular valueVisitor{ buffer };
		value.singleValue->apply_visitor(valueVisitor);
	}
	else if (value.multiValue) {
		buffer.SerializeAs<Cry::Byte>(ValueSelectType::MULTI);
		ValuePackerMulti valueVisitor{ buffer };
		value.multiValue->apply_visitor(valueVisitor);
	}
	else if (value.recordValue) {
		buffer.SerializeAs<Cry::Byte>(ValueSelectType::RECORD);
		RecordValue::Serialize(value.recordValue.get(), buffer);
	}
	else if (value.referenceValue) {
		buffer.SerializeAs<Cry::Byte>(ValueSelectType::REFERENCE);
		PointerPacker(value.referenceValue, buffer);
	}
}

// Convert byte stream into value.
void Value::ValueSelect::Unpack(ValueSelect& value, Cry::ByteArray& buffer)
{
	switch (static_cast<ValueSelectType>(buffer.Deserialize<Cry::Byte>(Cry::ByteArray::AUTO)))
	{
	case ValueSelectType::SINGLE: {
		ValuePackerSingular valueVisitor{ buffer };
		value = ValueSelect{ valueVisitor.Load() };
		break;
	}
	case ValueSelectType::MULTI: {
		ValuePackerMulti valueVisitor{ buffer };
		value = ValueSelect{ valueVisitor.Load() };
		break;
	}
	case ValueSelectType::RECORD: {
		RecordValue recordVal;
		RecordValue::Deserialize(recordVal, buffer);
		value = ValueSelect{ std::move(recordVal) };
		break;
	}
	case ValueSelectType::REFERENCE: {
		Value refVal = ::Util::MakeInt(0); //TODO: uninitialized value
		PointerUnpacker(refVal, buffer);
		value = ValueSelect{ std::move(refVal) };
		break;
	}
	default:
		break;
	}
}

// Convert value in string.
std::string Value::ValueSelect::ToString() const
{
	if (singleValue) {
		ConvertToStringSingular stringVisitor;
		singleValue->apply_visitor(stringVisitor);
		return stringVisitor.output;
	}
	else if (multiValue) {
		ConvertToStringMulti stringVisitor;
		multiValue->apply_visitor(stringVisitor);
		return stringVisitor.output;
	}
	else if (recordValue) {
		return recordValue->ToString();
	}
	else if (referenceValue) {
		//TODO: print should be ToString
		return referenceValue->Print();
	}

	CryImplExcept(); //TODO
}

// Serialize the value into byte array.
Cry::ByteArray Value::Serialize() const
{
	Cry::ByteArray buffer;
	Serialize((*this), buffer);
	return buffer;
}

void Value::Serialize(const Value& value, Cry::ByteArray& buffer)
{
	buffer.SetMagic(VALUE_MAGIC);
	buffer.SetPlatformCompat();

	// Serialzie type.
	Typedef::TypeFacade::Serialize(value.m_internalType, buffer);

	// Serialzie value.
	ValueSelect::Pack(value.m_value, buffer);
}

void Value::Deserialize(Value& value, Cry::ByteArray& buffer)
{
	if (!buffer.ValidateMagic(VALUE_MAGIC)) {
		CryImplExcept(); //TODO
	}

	if (!buffer.IsPlatformCompat()) {
		CryImplExcept(); //TODO
	}

	// Convert stream to type.
	Typedef::TypeFacade::Deserialize(value.m_internalType, buffer);

	// Convert stream into value.
	ValueSelect::Unpack(value.m_value, buffer);
}

// Copy other value into this value, but leave the
// type as is.
Value& Value::operator=(const Value& other)
{
	if (*this == other) { return (*this); }

	if (m_internalType != other.m_internalType) {
		throw InvalidTypeCastException{};
	}

	m_value = other.m_value;
	return (*this);
}

// Move other value into this value, but leave the
// type as is.
Value& Value::operator=(Value&& other)
{
	if (*this == other) { return (*this); }

	if (m_internalType != other.m_internalType) {
		throw InvalidTypeCastException{};
	}

	m_value = std::move(other.m_value);
	return (*this);
}

// Comparison equal operator.
bool Value::operator==(const Value& other) const
{
	return Empty() == other.Empty()
		&& m_value == other.m_value
		&& m_internalType == other.m_internalType;
}

// Comparison not equal operator.
bool Value::operator!=(const Value& other) const
{
	return !operator==(other);
}

// Stream value to ostream.
std::ostream& operator<<(std::ostream& os, const Value& value)
{
	os << value.Print();
	return os;
}

} // namespace Valuedef
} // namespace SubValue
} // namespace CryCC

namespace Util
{

using namespace CryCC::SubValue::Valuedef;

// Evaluate value as boolean if conversion is possible. If the conversion
// is not possible, an exception is thrown and caught here. In that case
// the evaluator returns with a negative result.
bool EvaluateValueAsBoolean(const Value& value)
{
	try { return value.As<int>(); }
	catch (const Value::InvalidTypeCastException&) {}
	return false;
}

// Convert value as integer. If the conversion fails an exception
// is thrown upwards to the caller.
int EvaluateValueAsInteger(const Value& value)
{
	return value.As<int>();
}

Value ValueFactory::MakeValue(Cry::ByteArray& buffer)
{
	Value value = Util::MakeInt(0); //TODO: Make uninitialized
	Value::Deserialize(value, buffer);
	return value;
}

} // namespace Util
