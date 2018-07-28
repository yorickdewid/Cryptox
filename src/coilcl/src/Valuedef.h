// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "TypeFacade.h"
#include "RecordValue.h"

#include <Cry/Serialize.h>

#include <boost/variant.hpp>
#include <boost/optional.hpp>
#include <boost/lexical_cast.hpp>

#include <string>

//TODO:
// - Cleanup old obsolete code
// - IsConvertable/Converter

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
		&& !std::is_same<Type, RecordValue>::value
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
	using ValueVariantSingle = boost::variant<int, char, float, double, bool>; //TODO: rename
	using ValueVariantMulti = boost::variant<std::vector<int> //TODO: rename
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
			: runtime_error{ "" } //TODO:
		{
		}
	};
	struct UninitializedValueException : public std::runtime_error
	{
		explicit UninitializedValueException()
			: runtime_error{ "" } //TODO:
		{
		}
	};

protected:
	// The internal datastructure stores the value
	// as close to the actual data type specifier.
	struct ValueSelect final
	{
		ValueSelect() = default; //TODO: for now
		ValueSelect(const ValueSelect&) = default;
		ValueSelect(ValueSelect&&) = default;
		ValueSelect(ValueVariantSingle value)
			: singleValue{ value }
		{
		}

		ValueSelect(ValueVariantMulti value)
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

		// Check if all of the values are empty
		bool Empty() const noexcept
		{
			return !singleValue
				&& !multiValue
				&& !recordValue
				&& !referenceValue;
		}

		// Comparison equal operator
		bool operator==(const ValueSelect& other) const
		{
			return singleValue == other.singleValue
				&& multiValue == other.multiValue
				&& recordValue == other.recordValue
				&& referenceValue == other.referenceValue;
		}
		// Comparison not equal operator
		bool operator!=(const ValueSelect& other) const
		{
			return !operator==(other);
		}

		// Assign ValueSelect
		ValueSelect& operator=(const ValueSelect&);
		// Assign ValueSelect
		ValueSelect& operator=(ValueSelect&&);

		// Convert value into byte stream
		static void Pack(const ValueSelect&, Cry::ByteArray&);
		// Convert byte stream into value
		static void Unpack(ValueSelect&, Cry::ByteArray&);

		// Return value as string
		std::string ToString() const;

		// Check if an value was set
		operator bool() const { return !Empty(); }

		// Clear any values
		void Clear();

		boost::optional<ValueVariantSingle> singleValue;
		boost::optional<ValueVariantMulti> multiValue;
		boost::optional<RecordValue> recordValue;
		std::shared_ptr<Value> referenceValue;
	} m_value;

private:
	template<typename CastTypePart>
	CastTypePart ValueCastImp(...) const;

	// Try cast on native types or throw predefined exception.
	template<typename ValueCastImp, typename std::enable_if<Trait::IsNativeSingleType<ValueCastImp>::value>::type* = nullptr>
	ValueCastImp ValueCastImp(int) const
	{
		try {
			if (m_value.singleValue) {
				return boost::get<ValueCastImp>(m_value.singleValue.get());
			}
			else {
				throw UninitializedValueException{};
			}
		}
		catch (const boost::bad_get&) {
			throw InvalidTypeCastException{};
		}
	}

	// Try cast on container types throw predefined exception.
	template<typename ValueCastImp, typename std::enable_if<Trait::IsNativeMultiType<ValueCastImp>::value>::type* = nullptr>
	ValueCastImp ValueCastImp(int) const
	{
		try {
			if (m_value.multiValue) {
				return boost::get<ValueCastImp>(m_value.multiValue.get());
			}
			else {
				throw UninitializedValueException{};
			}
		}
		catch (const boost::bad_get&) {
			throw InvalidTypeCastException{};
		}
	}

	// Try cast on container types throw predefined exception.
	template<typename ValueCastImp, typename std::enable_if<std::is_same<ValueCastImp, RecordValue>::value>::type* = nullptr>
	ValueCastImp ValueCastImp(int) const
	{
		try {
			if (m_value.recordValue) {
				return boost::get<ValueCastImp>(m_value.recordValue.get());
			}
			else {
				throw UninitializedValueException{};
			}
		}
		catch (const boost::bad_get&) {
			throw InvalidTypeCastException{};
		}
	}

	// Try cast on container types throw predefined exception.
	template<typename ValueCastImp, typename std::enable_if<std::is_same<ValueCastImp, Value>::value>::type* = nullptr>
	ValueCastImp ValueCastImp(int) const
	{
		try {
			if (m_value.referenceValue) {
				return (*m_value.referenceValue.get());
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
	Value(const Value&) = default;
	Value(Value&&) = default;

	// Value declaration without initialization.
	Value(Typedef::TypeFacade);
	// Value declaration and initialization.
	Value(Typedef::TypeFacade, ValueVariantSingle&&);
	// Value declaration and initialization.
	Value(Typedef::TypeFacade, ValueVariantMulti&&);
	// Value declaration and initialization.
	Value(Typedef::TypeFacade, RecordValue&&);
	// Pointer value declaration and initialization.
	Value(Typedef::TypeFacade, Value&&);

	// Access type information.
	Typedef::TypeFacade Type() const { return m_internalType; }

	// Check if current value is an pointer.
	inline bool IsReference() const { return !!m_value.referenceValue; } //TODO: refactor & remove in lieu of Typedef
	// Check if value is empty.
	inline bool Empty() const noexcept { return m_value.Empty(); }

	//TODO: REMOVE: OBSOLETE:
	inline bool IsArray() const noexcept { return false; }

	// If the cast fails a InvalidTypeCastException is thrown.
	template<typename CastType>
	inline CastType As() const
	{
		return ValueCastImp<CastType>(int{});
	}

	//FUTURE: replace with global Cry::ToString()
	//TODO: replace with ToString();
	// Return value as string.
	virtual const std::string Print() const
	{
		return m_value.ToString();
	}

	// Serialize the value into byte array.
	Cry::ByteArray Serialize() const;

	// Serialize the value into byte array.
	static void Serialize(const Value&, Cry::ByteArray&);
	// Serialize byte array into value.
	static void Deserialize(Value&, Cry::ByteArray&);

	// Copy other value into this value.
	Value& operator=(const Value&);
	// Move other value into this value.
	Value& operator=(Value&&);

	// Comparison equal operator.
	bool operator==(const Value&) const;
	// Comparison not equal operator.
	bool operator!=(const Value&) const;

	// Check if an value was set.
	inline operator bool() const { return !Empty(); }
	// Check if an value was set.
	inline bool operator!() const { return Empty(); }

	// Stream value to ostream.
	friend std::ostream& operator<<(std::ostream&, const Value&);

private:
	Typedef::TypeFacade m_internalType;
};

template<>
inline std::string Value::As() const
{
	const auto value = ValueCastImp<std::vector<char>>(int{});
	return std::string{ value.cbegin(), value.cend() };
}

static_assert(std::is_copy_constructible<Value>::value, "Value !is_copy_constructible");
static_assert(std::is_move_constructible<Value>::value, "Value !is_move_constructible");
static_assert(std::is_copy_assignable<Value>::value, "Value !is_copy_assignable");
static_assert(std::is_move_assignable<Value>::value, "Value !is_move_assignable");

namespace Detail
{

using namespace Typedef;

struct ValueDeductor
{
	template<BuiltinType::Specifier Specifier, typename NativeRawType>
	Valuedef::Value MakeValue(NativeRawType value)
	{
		return Valuedef::Value{TypeFacade{ Util::MakeBuiltinType(Specifier) }
			, Valuedef::Value::ValueVariantSingle{ value } };
	}

	template<BuiltinType::Specifier Specifier, typename NativeRawType>
	Valuedef::Value MakeMultiValue(NativeRawType&& value)
	{
		return Valuedef::Value{TypeFacade{ Util::MakeBuiltinType(Specifier) }
			, Valuedef::Value::ValueVariantMulti{ std::move(value) } };
	}

	template<typename PlainType>
	Valuedef::Value ConvertNativeType(PlainType value);

	template<typename NativeType>
	void DeduceTypeQualifier(Valuedef::Value internalValue, NativeType&&)
	{
		if (std::is_const<NativeType>::value) {
			internalValue.Type()->SetQualifier(TypedefBase::TypeQualifier::CONST_T);
		}
		if (std::is_volatile<NativeType>::value) {
			internalValue.Type()->SetQualifier(TypedefBase::TypeQualifier::VOLATILE);
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
	return MakeValue<BuiltinType::Specifier::INT>(value);
}
template<>
inline Valuedef::Value ValueDeductor::ConvertNativeType(char value)
{
	return MakeValue<BuiltinType::Specifier::CHAR>(value);
}
template<>
inline Valuedef::Value ValueDeductor::ConvertNativeType(float value)
{
	return MakeValue<BuiltinType::Specifier::FLOAT>(value);
}
template<>
inline Valuedef::Value ValueDeductor::ConvertNativeType(double value)
{
	return MakeValue<BuiltinType::Specifier::DOUBLE>(value);
}
template<>
inline Valuedef::Value ValueDeductor::ConvertNativeType(bool value)
{
	return MakeValue<BuiltinType::Specifier::BOOL>(value);
}
template<>
inline Valuedef::Value ValueDeductor::ConvertNativeType(Valuedef::Value value)
{
	return Valuedef::Value{ TypeFacade{ Util::MakePointerType() }, std::move(value) };
}

//

template<>
inline Valuedef::Value ValueDeductor::ConvertNativeType(std::vector<int> value)
{
	return MakeMultiValue<BuiltinType::Specifier::INT>(value);
}
template<>
inline Valuedef::Value ValueDeductor::ConvertNativeType(std::vector<float> value)
{
	return MakeMultiValue<BuiltinType::Specifier::FLOAT>(value);
}
template<>
inline Valuedef::Value ValueDeductor::ConvertNativeType(std::vector<double> value)
{
	return MakeMultiValue<BuiltinType::Specifier::DOUBLE>(value);
}
template<>
inline Valuedef::Value ValueDeductor::ConvertNativeType(std::vector<bool> value)
{
	return MakeMultiValue<BuiltinType::Specifier::BOOL>(value);
}

} // namespace Detail

} // namespace Valuedef

namespace Util
{

using namespace ::CoilCl;

struct ValueFactory
{
	static Valuedef::Value MakeValue(Cry::ByteArray&);
};

} // namespace Util
} // namespace CoilCl
