// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "Valuedef.h"
#include "ValueHelper.h"

#include <Cry/Cry.h>
#include <Cry/ByteOrder.h>

#define VALUE_MAGIC 0x7a

namespace CoilCl
{
namespace Valuedef
{

Value::Value()
	: m_objectType{ Util::MakeBuiltinType(Typedef::BuiltinType::Specifier::INT) }
{
}

Value::Value(Typedef::BaseType typeBase)
	: m_objectType{ typeBase }
{
}

Value::Value(Typedef::BaseType typeBase, ValueVariant value)
	: m_objectType{ typeBase }
	, m_value{ value }
{
}

// Value declaration without initialization
Value::Value(int, AST::TypeFacade typeBase)
	: m_internalType{ typeBase }
{
}
// Value declaration and initialization
Value::Value(int, AST::TypeFacade typeBase, ValueVariant2&& value)
	: m_internalType{ typeBase }
	, m_value3{ ValueSelect{ std::move(value) } }
{
}
// Value declaration and initialization
Value::Value(int, AST::TypeFacade typeBase, ValueVariant3&& value)
	: m_internalType{ typeBase }
	, m_value3{ ValueSelect{ std::move(value) } }
{
	m_internalType.SetArraySize(1);
}

// Value declaration and initialization
Value::Value(int, AST::TypeFacade typeBase, RecordValue&& value)
	: m_internalType{ typeBase }
	, m_value3{ ValueSelect{ std::move(value) } }
{
	auto *record = dynamic_cast<Typedef::RecordType *>(m_internalType.operator->());
	assert(record);

	//FUTURE: improve this structure and copy the record with transform to recordtype
	const auto recordValue = As2<RecordValue>();
	for (size_t i = 0; i < recordValue.Size(); ++i) {
		const auto fieldTypeFacade = std::make_shared<AST::TypeFacade>(recordValue.At(i)->Type());
		record->AddField(recordValue.FieldName(i), fieldTypeFacade);
	}
}

// Value declaration and initialization
Value::Value(int, AST::TypeFacade typeBase, Value&& value)
	: m_internalType{ typeBase }
	, m_value3{ ValueSelect{ std::move(value) } }
{
}

void Value::ValueSelect::Clear()
{
	singleValue = boost::none;
	multiValue = boost::none;
	recordValue = boost::none;
	referenceValue.reset();
	assert(Empty());
}

Value::ValueSelect& Value::ValueSelect::operator=(const ValueSelect& other)
{
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
	else {
		Clear();
	}

	return (*this);
}

Value::ValueSelect& Value::ValueSelect::operator=(ValueSelect&& other)
{
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
	else {
		Clear();
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

	Value::ValueVariant2 Load() const
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

	Value::ValueVariant3 Load() const
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

} // namespace

enum ValueSelectType
{
	SINGLE = 10,
	MULTI,
	RECORD,
	REFERENCE,
};

// Convert value into byte stream
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
		//TODO
	}
}

// Convert byte stream into value
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
		value = ValueSelect{ recordVal };
		break;
	}
	case ValueSelectType::REFERENCE:
		break;
	default:
		break;
	}
}

namespace
{

//TODO: OBSOLETE: REMOVE:
class TypePacker final : public boost::static_visitor<>
{
	enum NativeType : uint8_t
	{
		INT, CHAR, DOUBLE, STR,
	};

public:
	TypePacker(Cry::ByteArray& buffer)
		: m_buffer{ buffer }
	{
	}

	void operator()(int i) const
	{
		m_buffer.push_back(static_cast<NativeType>(NativeType::INT));
		m_buffer.push_back(static_cast<Cry::Byte>(i));
	}

	void operator()(char c) const
	{
		m_buffer.push_back(static_cast<NativeType>(NativeType::CHAR));
		m_buffer.push_back(c);
	}

	void operator()(double d) const
	{
		m_buffer.push_back(static_cast<NativeType>(NativeType::DOUBLE));
		m_buffer.push_back(static_cast<Cry::Byte>(d));
	}

	void operator()(const std::string& s) const
	{
		m_buffer.push_back(static_cast<NativeType>(NativeType::STR));
		m_buffer.push_back(static_cast<Cry::Byte>(s.size()));
		m_buffer.insert(m_buffer.cend(), s.begin(), s.end());
	}

	std::shared_ptr<Valuedef::Value> Unpack(Typedef::BaseType base)
	{
		switch (static_cast<NativeType>(m_buffer[0]))
		{
		case TypePacker::INT:
			return std::make_shared<Valuedef::Value>(std::move(base), static_cast<int>(m_buffer[1]));
		case TypePacker::CHAR:
			return std::make_shared<Valuedef::Value>(std::move(base), static_cast<char>(m_buffer[1]));
		case TypePacker::DOUBLE:
			return std::make_shared<Valuedef::Value>(std::move(base), static_cast<double>(m_buffer[1]));
		case TypePacker::STR: {
			auto strSize = static_cast<size_t>(m_buffer[1]);
			std::string buffer;
			buffer.resize(strSize);
			buffer.insert(buffer.cbegin(), m_buffer.begin() + 2, m_buffer.begin() + 2 + strSize);
			return std::make_shared<Valuedef::Value>(std::move(base), std::move(buffer));
		}
		default:
			CryImplExcept();
		}
	}

private:
	Cry::ByteArray& m_buffer;
};

} // namespace

//TODO: OBSOLETE: REMOVE:
const Cry::ByteArray Value::Serialize() const
{
	Cry::ByteArray buffer;
	buffer.SetMagic(VALUE_MAGIC);
	buffer.SetPlatformCompat();
	//buffer.SerializeAs<Cry::Byte>(m_isVoid);
	//buffer.SerializeAs<Cry::Short>(m_arraySize);//FUTURE: Limited to 16bits

	const auto type = m_objectType->TypeEnvelope();
	buffer.SerializeAs<Cry::Short>(type.size());
	buffer.insert(buffer.cend(), type.begin(), type.end());

	TypePacker visitor{ buffer };
	m_value.apply_visitor(visitor);
	return buffer;
}

// Serialize the value into byte array
Cry::ByteArray Value::Serialize(int) const
{
	Cry::ByteArray buffer;
	Serialize((*this), buffer);
	return buffer;
}

void Value::Serialize(const Value& value, Cry::ByteArray& buffer)
{
	buffer.SetMagic(VALUE_MAGIC);
	buffer.SetPlatformCompat();

	// Serialzie type
	AST::TypeFacade::Serialize(int{}, value.m_internalType, buffer);

	// Serialzie value
	ValueSelect::Pack(value.m_value3, buffer);
}

void Value::Deserialize(Value& value, Cry::ByteArray& buffer)
{
	if (!buffer.ValidateMagic(VALUE_MAGIC)) {
		CryImplExcept(); //TODO
	}

	if (!buffer.IsPlatformCompat()) {
		CryImplExcept(); //TODO
	}

	// Convert stream to type
	AST::TypeFacade::Deserialize(int{}, value.m_internalType, buffer);

	// Convert stream into value
	ValueSelect::Unpack(value.m_value3, buffer);
}

// Copy other value into this value
Value& Value::operator=(const Value& other)
{
	m_value3 = other.m_value3;
	return (*this);
}

// Move other value into this value
Value& Value::operator=(Value&& other)
{
	m_value3 = std::move(other.m_value3);
	return (*this);
}

// Comparison equal operator
bool Value::operator==(const Value& other) const
{
	return Empty() == other.Empty()
		&& m_value3 == other.m_value3
		&& m_internalType == other.m_internalType;
}

// Comparison not equal operator
bool Value::operator!=(const Value& other) const
{
	return !operator==(other);
}

std::ostream& operator<<(std::ostream& os, const Value& value)
{
	os << value.Print();
	return os;
}

} // namespace Valuedef

namespace Util
{

std::shared_ptr<CoilCl::Valuedef::Value> ValueCopy(const std::shared_ptr<CoilCl::Valuedef::Value>& value)
{
	return std::make_shared<CoilCl::Valuedef::Value>(*value);
}

bool EvaluateAsBoolean(std::shared_ptr<Valuedef::Value> value)
{
	//if (IsValueArray(value)) {
	//	CryImplExcept(); //TODO: cannot substitute array to void
	//}

	//FUTURE: this is expensive, replace try/catch
	try {
		return value->As<int>();
	}
	catch (...) {
		CryImplExcept();
	}

	return false;
}

bool EvaluateValueAsBoolean(const Value& /*value*/)
{
	CryImplExcept();
}

int EvaluateValueAsInteger(std::shared_ptr<Valuedef::Value> value)
{
	//if (IsValueArray(value)) {
	//	CryImplExcept(); //TODO: cannot substitute array to integer
	//}

	//FUTURE: this is expensive, replace try/catch
	//TODO: also cast double, float & char to int
	try {
		return value->As<int>();
	}
	catch (...) {}

	throw 1; //TODO:
}

int EvaluateValueAsInteger(const Value& /*value*/)
{
	CryImplExcept();
}

//
// ...
//

//TODO: OBSOLETE: REMOVE:
std::shared_ptr<Valuedef::Value> ValueFactory::BaseValue(Cry::ByteArray& buffer)
{
	buffer.StartOffset(0);
	if (!buffer.ValidateMagic(VALUE_MAGIC)) {
		CryImplExcept(); //TODO
	}

	if (!buffer.IsPlatformCompat()) {
		CryImplExcept(); //TODO
	}

	//bool isVoid = buffer.Deserialize<Cry::Byte>(Cry::ByteArray::AUTO);

	Cry::ByteArray type;
	size_t evSize = buffer.Deserialize<Cry::Short>(Cry::ByteArray::AUTO);
	type.resize(evSize);
	std::copy(buffer.cbegin() + buffer.Offset(), buffer.cbegin() + buffer.Offset() + evSize, type.begin());
	Typedef::BaseType ptr = Util::MakeType(std::move(type));
	if (!ptr) {
		CryImplExcept();
	}

	std::shared_ptr<Valuedef::Value> value;
	/*if (isVoid) {
		value = MakeVoid();
	}
	else {*/
	Cry::ByteArray subBuffer{ buffer.cbegin() + buffer.Offset() + evSize, buffer.cend() };
	Valuedef::TypePacker visitor{ subBuffer };
	value = visitor.Unpack(ptr);
	if (!value) {
		CryImplExcept();
	}
	//}

	return value;
}

Valuedef::Value ValueFactory::MakeValue(int, Cry::ByteArray& buffer)
{
	Valuedef::Value value;
	Valuedef::Value::Deserialize(value, buffer);
	return value;
}

} // namespace Util
} // namespace CoilCl
