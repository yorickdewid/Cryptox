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
// - Array value
// - Pointer value
// - Void value

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
	constexpr static const bool value = std::is_class<Type>::value;
};

} // namespace Trait

static_assert(Trait::IsNativeSingleType<int>::value, "");
static_assert(Trait::IsNativeSingleType<char>::value, "");
static_assert(Trait::IsNativeSingleType<float>::value, "");
static_assert(Trait::IsNativeSingleType<double>::value, "");
static_assert(Trait::IsNativeSingleType<bool>::value, "");

static_assert(Trait::IsNativeMultiType<std::vector<int>>::value, "");
static_assert(Trait::IsNativeMultiType<std::vector<char>>::value, "");
static_assert(Trait::IsNativeMultiType<std::vector<float>>::value, "");
static_assert(Trait::IsNativeMultiType<std::vector<double>>::value, "");
static_assert(Trait::IsNativeMultiType<std::vector<bool>>::value, "");

class Value //TODO: mark each value with an unique id
{
	friend struct Util::ValueFactory;

public:
	using ValueVariant = boost::variant<int, char, float, double, bool, std::string>; //OBSOLETE: REMOVE: TODO:
	using ValueVariant2 = boost::variant<int, char, float, double, bool>;
	using ValueVariant3 = boost::variant<std::vector<int>
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

	struct ValueSelect
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

		ValueSelect(Value&& value)
			: referenceValue{ std::make_shared<Value>(value) }
		{
		}

		bool Empty() const noexcept
		{
			return !singleValue && !multiValue && !referenceValue;
		}

		boost::optional<ValueVariant2> singleValue;
		boost::optional<ValueVariant3> multiValue;
		std::shared_ptr<Value> referenceValue;
	} m_value3;

	//FUTURE: May need to move to derived class
	// True if type is void
	bool m_isVoid{ false }; //TODO: why?

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
			else if (m_value3.referenceValue) {
				CryImplExcept();
				//TODO: deal with pointer
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

public:
	// Special member funcion, copy constructor
	Value(const Value&) = default;
	Value(Value&&) = default;
	Value(Typedef::BaseType typeBase);
	Value(Typedef::BaseType typeBase, ValueVariant value);

	template<typename NativeType>
	Value(Typedef::BaseType typeBase, NativeType&& value)
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
	inline bool IsArray() const { return !!m_value3.multiValue; }
	inline size_t Size() const noexcept { return 0; /* m_array.size();*/ }

	// Check if value is empty
	inline bool Empty() const noexcept { return m_value3.Empty(); }
	// Check if value is void
	inline bool IsVoid() const noexcept { return m_isVoid; } //TODO: remove

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

template<typename _Ty, typename _ = void>
class ValueObject;

template<typename _Ty>
using ValueType = std::shared_ptr<ValueObject<_Ty>>;

template<typename _Ty>
class ValueObject<_Ty, /*Trait::IsBuiltinType<_Ty>*/ void>
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

template<>
class ValueObject<void, void> : public Value
{
	using Specifier = Typedef::BuiltinType::Specifier;
	using _Myty = ValueObject<void>;

public:
	ValueObject()
		: Value{ Util::MakeBuiltinType(Specifier::VOID_T) }
	{
		m_isVoid = true;
		static_assert(sizeof(_Myty) == sizeof(Value), "");
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

	static ValueType<void> Deserialize(Cry::ByteArray&)
	{
		return std::make_shared<Valuedef::ValueObject<void>>();
	}
};

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
	CRY_UNUSED(value);
	return Valuedef::Value{ 0, AST::TypeFacade{ Util::MakePointerType() }, Valuedef::Value::ValueVariant2{ 12 } };
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

// The value helper functions ease the creation and modification of the
// value objects as well as some functions to query specific properties.
// In any case should the helper functions be used instead of accessing
// the value objects directly. The helper functions are designed to support
// the most common value types as such that the caller does not need to
// cast the value into a specific type.

namespace Util
{

using namespace ::CoilCl;

//
// Create explicit value with automatic type
//

template<typename NativeType, typename ValueType> //TODO: FIXME: DEPRECATED
inline auto MakeValueObject(Typedef::BuiltinType&& type, ValueType value)
{
	return std::make_shared<Valuedef::ValueObject<NativeType>>(std::move(type), value);
}
template<typename Type = std::string> //TODO: FIXME: DEPRECATED
inline Valuedef::ValueType<Type> MakeString(Type v)
{
	return MakeValueObject<Type>(Typedef::BuiltinType::Specifier::CHAR, v);
}
template<typename Type = int> //TODO: FIXME: DEPRECATED
inline Valuedef::ValueType<Type> MakeInt(Type v)
{
	return MakeValueObject<Type>(Typedef::BuiltinType::Specifier::INT, std::move(v));
}
template<typename Type = float> //TODO: FIXME: DEPRECATED
inline Valuedef::ValueType<Type> MakeFloat(Type v)
{
	return MakeValueObject<Type>(Typedef::BuiltinType::Specifier::FLOAT, std::move(v));
}
template<typename Type = double> //TODO: FIXME: DEPRECATED
inline Valuedef::ValueType<Type> MakeDouble(Type v)
{
	return MakeValueObject<Type>(Typedef::BuiltinType::Specifier::DOUBLE, std::move(v));
}
template<typename Type = char> //TODO: FIXME: DEPRECATED
inline Valuedef::ValueType<Type> MakeChar(Type v)
{
	return MakeValueObject<Type>(Typedef::BuiltinType::Specifier::CHAR, v);
}
template<typename Type = bool> //TODO: FIXME: DEPRECATED
inline Valuedef::ValueType<Type> MakeBool(Type v)
{
	return MakeValueObject<Type>(Typedef::BuiltinType::Specifier::BOOL, v);
}
template<typename Type = void> //TODO: why?
inline Valuedef::ValueType<Type> MakeVoid()
{
	return std::make_shared<Valuedef::ValueObject<Type>>();
}

//
// Create explicit value with automatic type
// Version 2.0
//

inline auto MakeString2(const std::string& v)
{
	std::vector<char> ve(v.begin(), v.end());
	ve.shrink_to_fit();
	const auto builtin = MakeBuiltinType(Typedef::BuiltinType::Specifier::CHAR);
	return Valuedef::Value{ 0, AST::TypeFacade{ builtin }, Valuedef::Value::ValueVariant3{ std::move(ve) } };
}
inline auto MakeInt2(int v)
{
	const auto builtin = MakeBuiltinType(Typedef::BuiltinType::Specifier::INT);
	return Valuedef::Value{ 0, AST::TypeFacade{ builtin }, Valuedef::Value::ValueVariant2{ std::move(v) } };
}
inline auto MakeFloat2(float v)
{
	const auto builtin = MakeBuiltinType(Typedef::BuiltinType::Specifier::FLOAT);
	return Valuedef::Value{ 0, AST::TypeFacade{ builtin }, Valuedef::Value::ValueVariant2{ std::move(v) } };
}
inline auto MakeDouble2(double v)
{
	const auto builtin = MakeBuiltinType(Typedef::BuiltinType::Specifier::DOUBLE);
	return Valuedef::Value{ 0, AST::TypeFacade{ builtin }, Valuedef::Value::ValueVariant2{ std::move(v) } };
}
inline auto MakeChar2(char v)
{
	const auto builtin = MakeBuiltinType(Typedef::BuiltinType::Specifier::CHAR);
	return Valuedef::Value{ 0, AST::TypeFacade{ builtin }, Valuedef::Value::ValueVariant2{ std::move(v) } };
}
inline auto MakeBool2(bool v)
{
	const auto builtin = MakeBuiltinType(Typedef::BuiltinType::Specifier::BOOL);
	return Valuedef::Value{ 0, AST::TypeFacade{ builtin }, Valuedef::Value::ValueVariant2{ std::move(v) } };
}
inline auto MakePointer(Valuedef::Value v)
{
	//Valuedef::Value::ValueVariant2{ std::move(v) };
	CRY_UNUSED(v);
	return Valuedef::Value{ 0, AST::TypeFacade{ MakePointerType() }, Valuedef::Value::ValueVariant2{ 12 } };
}
inline auto MakePointer(Valuedef::Value&& v)
{
	CRY_UNUSED(v);
	return Valuedef::Value{ 0, AST::TypeFacade{ MakePointerType() }, Valuedef::Value::ValueVariant2{ 12 } };
}

//
// Create explicit array value with automatic type
//

inline auto MakeIntArray(int v[])
{
	const auto builtin = MakeBuiltinType(Typedef::BuiltinType::Specifier::INT);
	std::vector<int> ve(v, v + sizeof(v) / sizeof(v[0]));
	return Valuedef::Value{ 0, AST::TypeFacade{ builtin }, Valuedef::Value::ValueVariant3{ std::move(ve) } };
}
inline auto MakeIntArray(std::vector<int> v)
{
	const auto builtin = MakeBuiltinType(Typedef::BuiltinType::Specifier::INT);
	return Valuedef::Value{ 0, AST::TypeFacade{ builtin }, Valuedef::Value::ValueVariant3{ v } };
}
inline auto MakeFloatArray(std::vector<float> v)
{
	const auto builtin = MakeBuiltinType(Typedef::BuiltinType::Specifier::FLOAT);
	return Valuedef::Value{ 0, AST::TypeFacade{ builtin }, Valuedef::Value::ValueVariant3{ v } };
}
inline auto MakeDoubleArray(std::vector<double> v)
{
	const auto builtin = MakeBuiltinType(Typedef::BuiltinType::Specifier::DOUBLE);
	return Valuedef::Value{ 0, AST::TypeFacade{ builtin }, Valuedef::Value::ValueVariant3{ v } };
}
inline auto MakeBoolArray(std::vector<bool> v)
{
	const auto builtin = MakeBuiltinType(Typedef::BuiltinType::Specifier::BOOL);
	return Valuedef::Value{ 0, AST::TypeFacade{ builtin }, Valuedef::Value::ValueVariant3{ v } };
}

//
// Create implicit value with automatic type
//

template<typename NativeType>
inline Valuedef::Value CaptureValueRaw(NativeType&& v)
{
	return Valuedef::Detail::ValueDeductor{}(std::forward<NativeType>(v));
}

//
// Change value internals
//

// Create deep copy of value with the same contents
std::shared_ptr<CoilCl::Valuedef::Value> ValueCopy(const std::shared_ptr<CoilCl::Valuedef::Value>&);

//
// Query value properties
//

// Evaluate the value as either true or false
bool EvaluateAsBoolean(std::shared_ptr<Valuedef::Value>);  //TODO: rename EvaluateValueAsBoolean
// Evaluate the value as an integer if possible
int EvaluateValueAsInteger(std::shared_ptr<Valuedef::Value>);
// Test if the value is a void type
bool IsValueVoid(std::shared_ptr<Valuedef::Value>);
// Test if the value is array type
bool IsValueArray(std::shared_ptr<Valuedef::Value>);
// Check if value is set
bool IsValueInitialized(std::shared_ptr<Valuedef::Value>);

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
