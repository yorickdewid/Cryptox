// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "Typedef.h"

#include <Cry/PolyConstructTrait.h>
#include <Cry/Serialize.h>

#include <boost/any.hpp>
#include <boost/lexical_cast.hpp>

#include <string>

namespace CoilCl
{
namespace Valuedef
{

class Value : public Cry::PolyConstruct
{
protected:
	using ValueVariant = boost::any;

	// The internal datastructure stores the value
	// as close to the actual data type specifier.
	ValueVariant m_value;

	// If this counter is greater than 0, the external type is an array
	struct
	{
		size_t m_Size = 0;
		bool _0terminator{ false };
	} m_array;

	// True for void types
	bool m_isVoid{ false };

public:
	// Special member funcion, copy constructor
	Value(const Value& other);
	Value(Typedef::BaseType typeBase, ValueVariant value);
	Value(Typedef::BaseType typeBase);

	// Value class is abstract and must be explicity defined
	virtual ~Value() = default; //TODO: = 0;

	// Return the type specifier
	Typedef::BaseType DataType() const noexcept { return m_objectType; }

	template<typename _CastTy>
	auto DataType() const { return std::dynamic_pointer_cast<_CastTy>(m_objectType); }

	// Check if current storage type is array
	inline auto IsArray() const noexcept { return m_array.m_Size != 0; }
	inline auto Size() const noexcept { return m_array.m_Size; }

	// By default try direct cast from any
	template<typename _Ty>
	_Ty As() const { return boost::any_cast<_Ty>(m_value); }

	// Print value
	virtual const std::string Print() const = 0;
	// Serialize the value into byte array
	virtual const Cry::ByteArray Serialize() const = 0;

private:
	Typedef::BaseType m_objectType;
};

// If string was required, try cast 'boost any' to vector and string
template<>
inline std::string Value::As() const
{
	const auto vec = boost::any_cast<std::vector<std::string::value_type>>(m_value);
	return m_array._0terminator ?
		std::string{ vec.cbegin(), vec.cend() - 1 } :
		std::string{ vec.cbegin(), vec.cend() };
}

namespace Trait {

template<typename _Ty>
using IsBuiltinType = typename std::enable_if<std::is_fundamental<_Ty>::value
	&& !std::is_void<_Ty>::value>::type;

template<typename _Ty>
using IsCompoundType = typename std::enable_if<std::is_compound<_Ty>::value
	&& !std::is_void<_Ty>::value && !std::is_enum<_Ty>::value
	&& !std::is_null_pointer<_Ty>::value && !std::is_function<_Ty>::value>::type;

} // namespace Trait

template<typename _Ty, typename _ = void>
class ValueObject;

template<typename _Ty>
using ValueType = std::shared_ptr<ValueObject<_Ty>>;

template<typename _Ty>
class ValueObject<_Ty, Trait::IsBuiltinType<_Ty>>
	: public Value
{
	using _Myty = ValueObject<_Ty>;

public:
	ValueObject(Typedef::BuiltinType&& type, _Ty value)
		: Value{ std::make_shared<Typedef::BuiltinType>(type), value }
	{
	}

	// Should never happen, but must implement contract
	_Myty *Construct() const { return nullptr; }

	// Copy self into new value object
	_Myty *Copy() const
	{
		return new _Myty{ (*this) };
	}

	virtual const std::string Print() const
	{
		return boost::lexical_cast<std::string>(boost::any_cast<_Ty>(m_value));
	}

	virtual const Cry::ByteArray Serialize() const
	{
		return DataType()->TypeEnvelope();
	}

	friend std::ostream& operator<<(std::ostream& os, const _Myty& value)
	{
		os << value.Print();
		return os;
	}

	static ValueType<_Ty> Deserialize(Cry::ByteArray& /*buffer*/)
	{
		_Ty _o = 12;
		return std::make_shared<Valuedef::ValueObject<_Ty>>(Typedef::BuiltinType::Specifier::INT, _o);
	}
};

template<typename _Ty>
class ValueObject<_Ty, Trait::IsCompoundType<_Ty>>
	: public Value
{
	using _Myty = ValueObject<_Ty>;

public:
	ValueObject(Typedef::BuiltinType&& type, _Ty value)
		: Value{ std::make_shared<Typedef::BuiltinType>(type) }
	{
		using container_type = typename std::remove_reference<decltype(value[0])>::type;

		std::vector<container_type> tmpArray;
		tmpArray.reserve(value.size() + 1);

		for (size_t i = 0; i < value.size(); ++i) {
			tmpArray.push_back(value[i]);
		}

		tmpArray.push_back('\0');
		m_array.m_Size = tmpArray.size();
		m_value = std::move(tmpArray);
		m_array._0terminator = true;
	}

	// Should never happen, but must implement contract
	_Myty *Construct() const { return nullptr; }

	// Copy self into new value object
	_Myty *Copy() const
	{
		return new _Myty{ (*this) };
	}

	virtual const std::string Print() const
	{
		return Value::As<std::string>();
	}

	virtual const Cry::ByteArray Serialize() const
	{
		return DataType()->TypeEnvelope();
	}

	friend std::ostream& operator<<(std::ostream& os, const _Myty& value)
	{
		os << value.Print();
		return os;
	}

	static ValueType<_Ty> Deserialize(Cry::ByteArray& /*buffer*/)
	{
		return std::make_shared<Valuedef::ValueObject<std::string>>(Typedef::BuiltinType::Specifier::CHAR, "lol");
	}
};

template<>
class ValueObject<void, void> : public Value
{
	using Specifier = Typedef::BuiltinType::Specifier;
	using _Myty = ValueObject<void>;

public:
	ValueObject()
		: Value{ Util::MakeBuiltinType(Specifier::VOID) }
	{
		m_isVoid = true;
	}

	// Should never happen, but must implement contract
	_Myty *Construct() const { return nullptr; }

	// Copy self into new value object
	_Myty *Copy() const
	{
		return new _Myty{ (*this) };
	}

	virtual const std::string Print() const
	{
		return typeid(void).name();
	}

	virtual const Cry::ByteArray Serialize() const
	{
		return DataType()->TypeEnvelope();
	}

	friend std::ostream& operator<<(std::ostream& os, const _Myty& value)
	{
		os << value.Print();
		return os;
	}

	static ValueType<void> Deserialize(Cry::ByteArray&)
	{
		return std::make_shared<Valuedef::ValueObject<void>>();
	}
};

} // namespace Valuedef

namespace Util
{

using namespace ::CoilCl;

template<typename _NativTy, typename _ValTy>
inline auto MakeValueObject(Typedef::BuiltinType&& type, _ValTy value)
{
	return std::make_shared<Valuedef::ValueObject<_NativTy>>(std::move(type), value);
}

template<typename _Ty = std::string, typename _TTy = Typedef::BuiltinType::Specifier::CHAR>
Valuedef::ValueType<_Ty> MakeString(_Ty v)
{
	return MakeValueObject<_Ty>(_TTy, std::move(v));
}

template<typename _Ty = int, typename _TTy = Typedef::BuiltinType::Specifier::INT>
Valuedef::ValueType<_Ty> MakeInt(_Ty v)
{
	return MakeValueObject<_Ty>(_TTy, std::move(v));
}

template<typename _Ty = void>
Valuedef::ValueType<_Ty> MakeVoid()
{
	return std::make_shared<Valuedef::ValueObject<_Ty>>();
}

//TODO:
inline auto CopyValueObject(const Valuedef::Value *object)
{
	assert(object);

	return nullptr;
}

} // namespace Util
} // namespace CoilCl
