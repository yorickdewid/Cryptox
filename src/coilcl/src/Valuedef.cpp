// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "Valuedef.h"

#include <Cry/ByteOrder.h>

#define VALUE_MAGIC 0x7a

namespace CoilCl
{
namespace Valuedef
{

Value::Value(Typedef::BaseType typeBase)
	: m_objectType{ typeBase }
{
}

Value::Value(Typedef::BaseType typeBase, ValueVariant value)
	: m_objectType{ typeBase }
	, m_value{ value }
{
}

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
			throw 1;
		}
	}

private:
	Cry::ByteArray& m_buffer;
};

const Cry::ByteArray Value::Serialize() const
{
	Cry::ByteArray buffer;
	buffer.SetMagic(VALUE_MAGIC);
	buffer.SetPlatformCompat();
	buffer.SerializeAs<Cry::Byte>(m_isVoid);
	buffer.SerializeAs<Cry::Short>(m_arraySize);//FUTURE: Limited to 16bits

	const auto type = m_objectType->TypeEnvelope();
	buffer.SerializeAs<Cry::Short>(type.size());
	buffer.insert(buffer.cend(), type.begin(), type.end());

	TypePacker visitor{ buffer };
	m_value.apply_visitor(TypePacker{ visitor });
	return buffer;
}

} // namespace Valuedef

namespace Util
{

bool EvaluateAsBoolean(std::shared_ptr<Valuedef::Value> value)
{
	if (IsVoid(value)) {
		throw 1; //TODO: void value is non orthogonal
	}

	//FUTURE: this is expensive, replace try/catch
	try {
		return value->As<int>();
	}
	catch (...) {}

	return false;
}

bool IsVoid(std::shared_ptr<Valuedef::Value> value)
{
	return value->IsVoid();
}

bool IsValueArray(std::shared_ptr<Valuedef::Value> value)
{
	return value->IsArray();
}

std::shared_ptr<Valuedef::Value> ValueFactory::BaseValue(Cry::ByteArray& buffer)
{
	buffer.StartOffset(0);
	if (!buffer.ValidateMagic(VALUE_MAGIC)) {
		throw 1; //TODO
	}

	if (!buffer.IsPlatformCompat()) {
		throw 2;//TODO
	}

	bool isVoid = buffer.Deserialize<Cry::Byte>(Cry::ByteArray::AUTO);
	size_t arraySize = buffer.Deserialize<Cry::Short>(Cry::ByteArray::AUTO);

	Cry::ByteArray type;
	size_t evSize = buffer.Deserialize<Cry::Short>(Cry::ByteArray::AUTO);
	type.resize(evSize);
	std::copy(buffer.cbegin() + buffer.Offset(), buffer.cbegin() + buffer.Offset() + evSize, type.begin());
	Typedef::BaseType ptr = Util::MakeType(std::move(type));
	if (!ptr) {
		throw 1;
	}

	std::shared_ptr<Valuedef::Value> value;
	if (isVoid) {
		value = MakeVoid();
	}
	else {
		Cry::ByteArray subBuffer{ buffer.cbegin() + buffer.Offset() + evSize, buffer.cend() };
		Valuedef::TypePacker visitor{ subBuffer };
		value = visitor.Unpack(ptr);
		if (!value) {
			throw 1;
		}
	}

	value->m_arraySize = arraySize;
	return value;
}

} // namespace Util
} // namespace CoilCl
