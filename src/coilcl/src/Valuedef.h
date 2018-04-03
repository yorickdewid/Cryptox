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
		bool _0terminator = false;
	} m_array;

	bool m_isVoid = false;

public:
	// Special member funcion, copy constructor
	Value(const Value& other);
	Value(Typedef::ValueType typeBase, ValueVariant value);
	Value(Typedef::ValueType typeBase);

	// Value class is abstract and must be explicity defined
	virtual ~Value() = default; // = 0;

	// Type specifier inputs
	inline void SetInline() { m_isInline = true; }

	// Return the type specifier
	Typedef::ValueType DataType() const noexcept { return m_objectType; }

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

private:
	bool m_isInline = false;
	Typedef::ValueType m_objectType;
};

template<typename _Ty, typename _ = void>
class ValueObject;

template<typename _Ty>
class ValueObject<_Ty,
	typename std::enable_if<std::is_fundamental<_Ty>::value
	&& !std::is_void<_Ty>::value>::type>
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

	virtual const std::string Print() const override
	{
		return boost::lexical_cast<std::string>(boost::any_cast<_Ty>(m_value));
	}

	friend std::ostream& operator<<(std::ostream& os, const _Myty& value)
	{
		os << value.Print();
		return os;
	}
};

template<typename _Ty>
class ValueObject<_Ty,
	typename std::enable_if<std::is_compound<_Ty>::value
	&& !std::is_void<_Ty>::value && !std::is_enum<_Ty>::value
	&& !std::is_null_pointer<_Ty>::value && !std::is_function<_Ty>::value>::type>
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

	virtual const std::string Print() const override
	{
		return Value::As<std::string>();
	}

	friend std::ostream& operator<<(std::ostream& os, const _Myty& value)
	{
		os << value.Print();
		return os;
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

	virtual const std::string Print() const override
	{
		return typeid(void).name();
	}

	friend std::ostream& operator<<(std::ostream& os, const _Myty& value)
	{
		os << value.Print();
		return os;
	}
};

} // namespace Valuedef

namespace Util
{

using namespace ::CoilCl;

template<typename _NativTy, typename _ValTy>
inline auto MakeValueObject(Typedef::BuiltinType&& type, _ValTy value)
{
	return std::make_unique<Valuedef::ValueObject<_NativTy>>(std::move(type), value); //TODO: Make shared
}

inline auto CopyValueObject(const Valuedef::Value *object)
{
	assert(object);

	return nullptr;
}

} // namespace Util
} // namespace CoilCl
