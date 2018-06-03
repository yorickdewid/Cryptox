// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "TypeFacade.h"
#include "Typedef.h" //TODO: remove
#include "RecordValue.h"

#include <Cry/Serialize.h>

#include <boost/any.hpp> //TODO: Why?
#include <boost/variant.hpp>
#include <boost/optional.hpp>
#include <boost/lexical_cast.hpp>

#include <string>

#ifdef CRY_DEBUG
#include <iostream>
#define DUMP_VALUE(v) \
	std::cout << v->DataType()->TypeName() << " >> " << v->Print() << std::endl;
#endif

#define CaptureValue(s) Util::CaptureValueRaw(std::move(s))

//TODO:
// - struct/union value
// - Serialize/Deserialize
// - Cleanup old obsolete code

namespace CoilCl
{
namespace Util
{

struct ValueFactory;

} // namespace Util

namespace Valuedef
{

namespace Trait {

template<typename Type>
struct IsAllowedType
{
	constexpr static const bool value = (std::is_fundamental<Type>::value
		|| std::is_same<Valuedef::Value, Type>::value
		|| std::is_class<Type>::value) //TODO: scope down to container
		&& !std::is_void<Type>::value && !std::is_function<Type>::value;
};

template<typename Type>
struct IsNativeSingleType
{
	constexpr static const bool value = std::is_fundamental<Type>::value;
};

template<typename Type>
struct IsNativeMultiType
{
	constexpr static const bool value = std::is_class<Type>::value
		&& !std::is_same<Type, Value>::value;
};

} // namespace Trait

static_assert(Trait::IsNativeSingleType<int>::value, "IsNativeSingleType failed");
static_assert(Trait::IsNativeSingleType<char>::value, "IsNativeSingleType failed");
static_assert(Trait::IsNativeSingleType<float>::value, "IsNativeSingleType failed");
static_assert(Trait::IsNativeSingleType<double>::value, "IsNativeSingleType failed");
static_assert(Trait::IsNativeSingleType<bool>::value, "IsNativeSingleType failed");

static_assert(Trait::IsNativeMultiType<std::vector<int>>::value, "IsNativeMultiType failed");
static_assert(Trait::IsNativeMultiType<std::vector<char>>::value, "IsNativeMultiType failed");
static_assert(Trait::IsNativeMultiType<std::vector<float>>::value, "IsNativeMultiType failed");
static_assert(Trait::IsNativeMultiType<std::vector<double>>::value, "IsNativeMultiType failed");
static_assert(Trait::IsNativeMultiType<std::vector<bool>>::value, "IsNativeMultiType failed");

class Value //TODO: mark each value with an unique id
{
	friend struct Util::ValueFactory;

public:
	using ValueVariant = boost::variant<int, char, float, double, bool, std::string>; //OBSOLETE: REMOVE: TODO:
	using ValueVariant2 = boost::variant<int, char, float, double, bool>; //TODO: rename
	using ValueVariant3 = boost::variant<std::vector<int> //TODO: rename
		, std::vector<char>
		, std::vector<float>
		, std::vector<double>
		, std::vector<bool>>;

	//
	// Local exceptions
	//

	struct InvalidTypeCastException : public std::runtime_error
	{
		explicit InvalidTypeCastException()
			: runtime_error{ "" }
		{
		}
	};
	struct UninitializedValueException : public std::runtime_error
	{
		explicit UninitializedValueException()
			: runtime_error{ "" }
		{
		}
	};

protected:
	// The internal datastructure stores the value
	// as close to the actual data type specifier.
	ValueVariant m_value; //OBSOLETE: REMOVE: TODO:

	struct ValueSelect final
	{
		ValueSelect() = default; //TODO: for now
		ValueSelect(ValueVariant2 value)
			: singleValue{ value }
		{
		}

		ValueSelect(ValueVariant3 value)
			: multiValue{ value }
		{
		}

		ValueSelect(RecordValue value)
			: recordValue{ value }
		{
		}

		ValueSelect(Value&& value)
			: referenceValue{ std::make_shared<Value>(value) }
		{
		}

		bool Empty() const noexcept
		{
			return !singleValue
				&& !multiValue
				&& !recordValue
				&& !referenceValue;
		}

		boost::optional<ValueVariant2> singleValue;
		boost::optional<ValueVariant3> multiValue;
		boost::optional<RecordValue> recordValue;
		std::shared_ptr<Value> referenceValue;
	} m_value3;

	struct ConvertToStringVisitor final : public boost::static_visitor<>
	{
		std::string output;

		template<typename NativeType>
		void operator()(NativeType& value)
		{
			output = boost::lexical_cast<std::string>(value);
		}
	};

private:
	template<typename CastTypePart>
	CastTypePart ValueCastImp(...) const;

	// Try cast on native types or throw predefined exception
	template<typename ValueCastImp, typename std::enable_if<Trait::IsNativeSingleType<ValueCastImp>::value>::type* = nullptr>
	ValueCastImp ValueCastImp(int) const
	{
		try {
			if (m_value3.singleValue) {
				return boost::get<ValueCastImp>(m_value3.singleValue.get());
			}
			else {
				throw UninitializedValueException{};
			}
		}
		catch (const boost::bad_get&) {
			throw InvalidTypeCastException{};
		}
	}

	// Try cast on container types throw predefined exception
	template<typename ValueCastImp, typename std::enable_if<Trait::IsNativeMultiType<ValueCastImp>::value>::type* = nullptr>
	ValueCastImp ValueCastImp(int) const
	{
		try {
			if (m_value3.multiValue) {
				return boost::get<ValueCastImp>(m_value3.multiValue.get());
			}
			else {
				throw UninitializedValueException{};
			}
		}
		catch (const boost::bad_get&) {
			throw InvalidTypeCastException{};
		}
	}

	// Try cast on container types throw predefined exception
	template<typename ValueCastImp, typename std::enable_if<std::is_same<ValueCastImp, Value>::value>::type* = nullptr>
	ValueCastImp ValueCastImp(int) const
	{
		try {
			if (m_value3.referenceValue) {
				return (*m_value3.referenceValue.get());
			}
			else {
				throw UninitializedValueException{};
			}
		}
		catch (const boost::bad_get&) {
			throw InvalidTypeCastException{};
		}
	}

public:
	// Special member funcion, copy constructor
	Value(const Value&) = default;
	Value(Value&&) = default;
	Value(Typedef::BaseType typeBase); //TODO: remove obsolete
	Value(Typedef::BaseType typeBase, ValueVariant value); //TODO: remove obsolete

	template<typename NativeType>
	Value(Typedef::BaseType typeBase, NativeType&& value) //TODO: remove obsolete
		: m_objectType{ typeBase }
		, m_value{ ValueVariant{ std::forward<NativeType>(value) } }
	{
	}

	// Value declaration without initialization
	Value(int, AST::TypeFacade);
	// Value declaration and initialization
	Value(int, AST::TypeFacade, ValueVariant2&&);
	// Value declaration and initialization
	Value(int, AST::TypeFacade, ValueVariant3&&);
	// Value declaration and initialization
	Value(int, AST::TypeFacade, RecordValue&&);
	// Pointer value declaration and initialization
	Value(int, AST::TypeFacade, Value&&);

	// Swap-in native replacement value
	template<typename NativeType>
	void ReplaceValue(NativeType&& value) //TODO: fix name & allowed types
	{
		m_value = ValueVariant{ std::forward<NativeType>(value) };
	}
	// Copy value from another value
	void ReplaceValueWithValue(const Value& other) //TODO: fix name
	{
		m_value = other.m_value;
	}

	// Return the type specifier
	Typedef::BaseType DataType() const noexcept { return m_objectType; }

	template<typename CastType>
	auto DataType() const { return std::dynamic_pointer_cast<CastType>(m_objectType); }
	// Access type information
	AST::TypeFacade Type() const { return m_internalType; }

	// Check if current storage type is array
	inline bool IsArray() const { return !!m_value3.multiValue; } //TODO: refactor
	inline bool IsReference() const { return !!m_value3.referenceValue; } //TODO: refactor
	inline size_t Size() const noexcept { return 0; /* m_array.size();*/ } //TOOD:

	// Check if value is empty
	inline bool Empty() const noexcept { return m_value3.Empty(); }

	// By default try direct cast from variant, if the cast fails
	// a bad casting exception is thrown.
	template<typename CastType>
	CastType As() const { return boost::get<CastType>(m_value); }

	template<typename CastType>
	inline CastType As2() const
	{
		return ValueCastImp<CastType>(int{});
	}

	template<>
	std::string As2() const
	{
		const auto value = ValueCastImp<std::vector<char>>(int{});
		return std::string{ value.cbegin(), value.cend() };
	}

	//template<>
	//Value As2() const
	//{
	//	const auto value = ValueCastImp<Value>(int{});
	//	//return std::string{ value.cbegin(), value.cend() };
	//}

	//TODO: replace with global Cry::ToString()
	// Print value
	virtual const std::string Print() const
	{
		ConvertToStringVisitor conv;
		m_value.apply_visitor(conv);
		return conv.output;
	}

	// Serialize the value into byte array
	virtual const Cry::ByteArray Serialize() const;

private:
	Typedef::BaseType m_objectType; //TODO: replace with typefacade to account for pointers
	AST::TypeFacade m_internalType;
};

//TODO: OBSOLETE: REMOVE:
template<typename _Ty>
class ValueObject
	: public Value
{
	using _Myty = ValueObject<_Ty>;

public:
	ValueObject(Typedef::BuiltinType&& type, _Ty value)
		: Value{ std::make_shared<Typedef::BuiltinType>(type), value }
	{
		static_assert(sizeof(_Myty) == sizeof(Value), "");
	}

	virtual const std::string Print() const override
	{
		return boost::lexical_cast<std::string>(Value::As<_Ty>());
	}

	friend std::ostream& operator<<(std::ostream& os, const _Myty& value)
	{
		os << value.Print();
		return os;
	}
};

//TODO: OBSOLETE: REMOVE:
template<typename _Ty>
using ValueType = std::shared_ptr<ValueObject<_Ty>>;

namespace Detail
{

struct ValueDeductor
{
	template<Typedef::BuiltinType::Specifier Specifier, typename NativeRawType>
	Valuedef::Value MakeValue(NativeRawType value)
	{
		return Valuedef::Value{ 0
			, AST::TypeFacade{ Util::MakeBuiltinType(Specifier) }
			, Valuedef::Value::ValueVariant2{ value } };
	}

	template<Typedef::BuiltinType::Specifier Specifier, typename NativeRawType>
	Valuedef::Value MakeMultiValue(NativeRawType&& value)
	{
		return Valuedef::Value{ 0
			, AST::TypeFacade{ Util::MakeBuiltinType(Specifier) }
			, Valuedef::Value::ValueVariant3{ std::move(value) } };
	}

	template<typename PlainType>
	Valuedef::Value ConvertNativeType(PlainType value);

	template<typename NativeType>
	void DeduceTypeQualifier(Valuedef::Value internalValue, NativeType&&)
	{
		if (std::is_const<NativeType>::value) {
			internalValue.Type()->SetQualifier(Typedef::TypedefBase::TypeQualifier::CONST_T);
		}
		if (std::is_volatile<NativeType>::value) {
			internalValue.Type()->SetQualifier(Typedef::TypedefBase::TypeQualifier::VOLATILE);
		}
	}

public:
	template<typename NativeType>
	using RawType = typename std::decay<NativeType>::type;

	template<typename NativeType, typename = typename std::enable_if<Valuedef::Trait::IsAllowedType<NativeType>::value>::type>
	auto operator()(NativeType&& value)
	{
		const auto internalValue = ConvertNativeType<RawType<NativeType>>(const_cast<RawType<NativeType>&>(value));
		DeduceTypeQualifier(internalValue, std::forward<NativeType>(value));
		return internalValue;
	}
};

template<>
inline Valuedef::Value ValueDeductor::ConvertNativeType(int value)
{
	return MakeValue<Typedef::BuiltinType::Specifier::INT>(value);
}
template<>
inline Valuedef::Value ValueDeductor::ConvertNativeType(char value)
{
	return MakeValue<Typedef::BuiltinType::Specifier::CHAR>(value);
}
template<>
inline Valuedef::Value ValueDeductor::ConvertNativeType(float value)
{
	return MakeValue<Typedef::BuiltinType::Specifier::FLOAT>(value);
}
template<>
inline Valuedef::Value ValueDeductor::ConvertNativeType(double value)
{
	return MakeValue<Typedef::BuiltinType::Specifier::DOUBLE>(value);
}
template<>
inline Valuedef::Value ValueDeductor::ConvertNativeType(bool value)
{
	return MakeValue<Typedef::BuiltinType::Specifier::BOOL>(value);
}
template<>
inline Valuedef::Value ValueDeductor::ConvertNativeType(Valuedef::Value value)
{
	return Valuedef::Value{ 0, AST::TypeFacade{ Util::MakePointerType() }, std::move(value) };
}

//

template<>
inline Valuedef::Value ValueDeductor::ConvertNativeType(std::vector<int> value)
{
	return MakeMultiValue<Typedef::BuiltinType::Specifier::INT>(value);
}
template<>
inline Valuedef::Value ValueDeductor::ConvertNativeType(std::vector<float> value)
{
	return MakeMultiValue<Typedef::BuiltinType::Specifier::FLOAT>(value);
}
template<>
inline Valuedef::Value ValueDeductor::ConvertNativeType(std::vector<double> value)
{
	return MakeMultiValue<Typedef::BuiltinType::Specifier::DOUBLE>(value);
}
template<>
inline Valuedef::Value ValueDeductor::ConvertNativeType(std::vector<bool> value)
{
	return MakeMultiValue<Typedef::BuiltinType::Specifier::BOOL>(value);
}

} // namespace Detail

} // namespace Valuedef

namespace Util
{

using namespace ::CoilCl;

struct ValueFactory
{
	static std::shared_ptr<Valuedef::Value> BaseValue(Cry::ByteArray&);

	template<typename _Ty>
	static std::shared_ptr<CoilCl::Valuedef::ValueObject<_Ty>> MakeValue(Cry::ByteArray& buffer)
	{
		auto basePtr = BaseValue(buffer);
		return std::static_pointer_cast<CoilCl::Valuedef::ValueObject<_Ty>>(basePtr);
	}
};

} // namespace Util
} // namespace CoilCl
