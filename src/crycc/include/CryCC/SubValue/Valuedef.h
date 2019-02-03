// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

// Project includes.
#include <CryCC/SubValue/TypeFacade.h>
#include <CryCC/SubValue/ValueContract.h>
#include <CryCC/SubValue/NilValue.h>

// Framework includes.
#include <Cry/Serialize.h>

#include <boost/variant.hpp>
#include <boost/optional.hpp>
#include <boost/lexical_cast.hpp>

// Language includes.
#include <string>

//TODO:
// - Cleanup old obsolete code

#ifdef _OBSOLETE_

namespace Util
{

struct ValueFactory;

} // namespace Util

#endif // _OBSOLETE_

namespace CryCC::SubValue::Valuedef
{

#ifdef _OBSOLETE_

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

#endif // _OBSOLETE_

#ifdef _OBSOLETE_

class Value //TODO: mark each value with an unique id
{
	friend struct Util::ValueFactory;

public:
	//TODO: move into ValueSelect
	using ValueVariantSingle = boost::variant<int, char, float, double, bool>; //TODO: rename
	using ValueVariantMulti = boost::variant<std::vector<int> //TODO: rename
		, std::vector<char>
		, std::vector<float>
		, std::vector<double>
		, std::vector<bool>>;

	//
	// Local exceptions.
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

		//
		// Value category initializers.
		//

		ValueSelect(ValueVariantSingle);
		ValueSelect(ValueVariantMulti);
		ValueSelect(RecordValue&&);
		ValueSelect(Value&&);

		// Check if all values are empty.
		bool Empty() const noexcept
		{
			return !singleValue
				&& !multiValue
				&& !recordValue
				&& !referenceValue;
		}

		// Comparison equal operator.
		bool operator==(const ValueSelect& other) const
		{
			return singleValue == other.singleValue
				&& multiValue == other.multiValue
				&& recordValue == other.recordValue
				&& referenceValue == other.referenceValue;
		}

		// Comparison not equal operator.
		bool operator!=(const ValueSelect& other) const
		{
			return !operator==(other);
		}

		// Assign ValueSelect.
		ValueSelect& operator=(const ValueSelect&);
		// Assign ValueSelect.
		ValueSelect& operator=(ValueSelect&&);

		// Convert value into byte stream.
		static void Pack(const ValueSelect&, Cry::ByteArray&);
		// Convert byte stream into value.
		static void Unpack(ValueSelect&, Cry::ByteArray&);

		// Return value as string.
		std::string ToString() const;

		// Check if an value was set.
		operator bool() const { return !Empty(); }

		// Clear any values.
		void Clear();

		//
		// Only one of these value categories can be initialized. The values 
		// implement the value contract. The value selector does not determine how the
		// value category process the inner state. A value category can hold one or many
		// value variant.
		//

		boost::optional<ValueVariantSingle> singleValue; //TODO: rename singleNativeValue
		boost::optional<ValueVariantMulti> multiValue; //TODO: rename multiNativeValue
		boost::optional<RecordValue> recordValue; //TODO: rename singleRecordValue
		//boost::optional<std::vector<RecordValue>> multiRecordValue;
		std::shared_ptr<Value> referenceValue; //TODO: rename singleReferenceValue
		//std::shared_ptr<std::vector<Value>> multiReferenceValue;
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
	Value(Typedef::TypeFacade, ValueVariantSingle&&);
	Value(Typedef::TypeFacade, ValueVariantMulti&&, size_t elements);
	Value(Typedef::TypeFacade, RecordValue&&);
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

#endif // _OBSOLETE_

// Compiler framework internal value representation.
//
// A value can be construed as a type specifier and an optional data block.
// Value always points to a type, becuse values cannot be without context. There
// are types that serve special perposes, such as empty types. The is always
// of the type facade kind. This class utilizes the facade to request internal
// type information. In addition to a type there is space for a value category.
// The value category is optional from the caller perspective, but is always present
// in the value intrinsics. A value category holds a block of data representing
// the value. The value category is bound by a tight value category contract in
// order for value category to be extendable.
//
// Is it not recommended to create the value directly, but to use a value helper
// instead. The recommended solution is API sustainable.
class Value
{
#ifdef _OBSOLETE_
	friend struct Util::ValueFactory;
#endif // _OBSOLETE_

public:
	using default_value_type = NilValue;

private:
	struct ProxyInterface : public ValueContract
	{
		// Check if value is set.
		virtual bool IsInitialized() const noexcept = 0;
		// Compare the proxy interface.
		virtual bool Compare(ProxyInterface&) const = 0;
		// Retrieve the value category identifier.
		virtual int Id() const = 0;
		// Clone the abstract proxy
		virtual std::unique_ptr<ProxyInterface> Clone() const = 0;
		// Serialize the value.
		virtual void Serialize(Cry::ByteArray& buffer) const = 0;
		// Attach type facade to value.
		virtual void ReferenceType(const Typedef::TypeFacade*) = 0;
		// Signal end of initialization.
		virtual void ValueInit() = 0;
		// Trigger addition on the value.
		virtual void Increment(int inc = 1) = 0;
		// Trigger subtraction on the value.
		virtual void Decrement(int dec = 1) = 0;

		//
		// Arithmetic operations.
		//

		enum ArithOperation { PLUS, MINUS, MULTIPLIES, DIVIDES, MODULUS };

		virtual Value ArithOperationValue(ArithOperation, const Typedef::TypeFacade&, ProxyInterface&) const = 0;
	};

	template<typename ValueType, typename = typename std::enable_if<IsValueContractCompliable<ValueType>::value>::type>
	class AbstractValueProxy : public ProxyInterface
	{
		template<template<typename> typename BinaryOperation>
		auto ConstructValueFromOperator(Typedef::TypeFacade type, ProxyInterface& other) const
		{
			if (this->Id() != other.Id()) { CryImplExcept(); }
			auto& otherProxy = dynamic_cast<AbstractValueProxy<ValueType>&>(other);
			ValueType value = std::invoke(BinaryOperation<ValueType>(), m_innerValue, otherProxy.m_innerValue);
			return Value{ std::move(type), std::move(value) };
		}

	protected:
		ValueType m_innerValue;

	public:
		using value_type = ValueType;
		using compose_type = AbstractValueProxy<ValueType>;
		using buffer_type = typename ValueType::buffer_type;
		using typdef_type = typename ValueType::typdef_type;
		using value_category = typename ValueType::value_category;

		explicit AbstractValueProxy() = default;
		AbstractValueProxy(const AbstractValueProxy&) = default;

		template<typename... ArgTypes>
		AbstractValueProxy(ValueType&& valueCategory, ArgTypes... args)
			: m_innerValue{ std::forward<ValueType>(valueCategory) }
		{
		}

		virtual ~AbstractValueProxy() {}

		// Retrieve the value category identifier.
		int Id() const { return ValueType::value_category_identifier; }
		// Attach type facade to value.
		void ReferenceType(const Typedef::TypeFacade *ptr) { m_innerValue.ReferenceType(ptr); }
		// Signal end of initialization.
		void ValueInit() { m_innerValue.ValueInit(); }
		// Return the inner-value category.
		const ValueType& NativeValue() const { return m_innerValue; }
		// Return the inner-value category.
		ValueType& NativeValue() { return m_innerValue; }

		// Clone the abstract proxy.
		virtual std::unique_ptr<ProxyInterface> Clone() const
		{
			return std::make_unique<AbstractValueProxy<ValueType>>((*this));
		}

		// Serialize the value.
		virtual void Serialize(buffer_type& buffer) const
		{
			buffer.SerializeAs<Cry::Byte>(ValueType::value_category_identifier);
			ValueType::Serialize(m_innerValue, buffer);
		}

		// Forward the interface call to the value category.
		std::string ToString() const
		{
			return m_innerValue.ToString();
		}

		// Trigger addition on the value.
		void Increment(int inc) final { if (inc == 1) { m_innerValue++; } }
		// Trigger subtraction on the value.
		void Decrement(int dec) final { if (dec == 1) { m_innerValue--; } }

		// Check if value is set.
		bool IsInitialized() const noexcept
		{
			return ValueType::value_category_identifier != default_value_type::value_category_identifier;
		}

		// Compare the proxy interface.
		virtual bool Compare(ProxyInterface& other) const
		{
			if (this->Id() != other.Id()) { return false; }
			auto& otherProxy = dynamic_cast<AbstractValueProxy<ValueType>&>(other);
			return m_innerValue == otherProxy.m_innerValue;
		}

		// Arithmetic operations.
		Value ArithOperationValue(ArithOperation op, const Typedef::TypeFacade& type, ProxyInterface& other) const final
		{
			switch (op)
			{
			case PLUS:
				return ConstructValueFromOperator<std::plus>(type, other);
			case MINUS:
				return ConstructValueFromOperator<std::minus>(type, other);
			case MULTIPLIES:
				return ConstructValueFromOperator<std::multiplies>(type, other);
			case DIVIDES:
				return ConstructValueFromOperator<std::divides>(type, other);
			case MODULUS:
				return ConstructValueFromOperator<std::modulus>(type, other);
			}
			CryImplExcept();
		}
	};

	template<typename ValueType>
	class SingularValueProxy : public AbstractValueProxy<ValueType>
	{
	public:
		using base_type = AbstractValueProxy<ValueType>;
		using self_type = SingularValueProxy<ValueType>;

		explicit SingularValueProxy() = default;
		SingularValueProxy(const SingularValueProxy&) = default;

		template<typename... ArgTypes>
		SingularValueProxy(ValueType&& valueCategory, ArgTypes... args)
			: base_type{ std::forward<ValueType>(valueCategory) }
		{
		}

		// Clone the abstract proxy.
		virtual std::unique_ptr<ProxyInterface> Clone() const override
		{
			return std::make_unique<SingularValueProxy<ValueType>>((*this));
		}

		//
		// Proxy specific methods.
		//

		// Forward type cast to value category.
		//template<typename... CastTypes>
		auto Get() const
		{
			return m_innerValue.Get();
		}
	};

	template<typename ValueType>
	class MultiOrderValueProxy : public AbstractValueProxy<ValueType>
	{
	public:
		using base_type = AbstractValueProxy<ValueType>;
		using self_type = MultiOrderValueProxy<ValueType>;

		explicit MultiOrderValueProxy() = default;
		MultiOrderValueProxy(const MultiOrderValueProxy&) = default;

		template<typename... ArgTypes>
		MultiOrderValueProxy(ValueType&& valueCategory, ArgTypes... args)
			: base_type{ std::forward<ValueType>(valueCategory) }
		{
		}

		// Clone the abstract proxy.
		virtual std::unique_ptr<ProxyInterface> Clone() const override
		{
			return std::make_unique<MultiOrderValueProxy<ValueType>>((*this));
		}

		//
		// Proxy specific methods.
		//

		// Forward type cast to value category.
		template<typename... CastTypes>
		auto As() const
		{
			return m_innerValue.As<CastTypes...>();
		}
	};

	template<typename ValueType>
	class IterableValueProxy : public MultiOrderValueProxy<ValueType>
	{
	public:
		using base_type = MultiOrderValueProxy<ValueType>;
		using self_type = IterableValueProxy<ValueType>;
		using size_type = typename ValueType::size_type;

		explicit IterableValueProxy() = default;
		IterableValueProxy(const IterableValueProxy&) = default;

		template<typename... ArgTypes>
		IterableValueProxy(ValueType&& valueCategory, ArgTypes... args)
			: base_type{ std::forward<ValueType>(valueCategory) }
		{
		}

		// Clone the abstract proxy.
		virtual std::unique_ptr<ProxyInterface> Clone() const override
		{
			return std::make_unique<IterableValueProxy<ValueType>>((*this));
		}

		//
		// Proxy specific methods.
		//

		size_type Size() const { return m_innerValue.Size(); }
		bool Empty() const { return !this->Size(); }

		template<typename ReturnType>
		auto At(IterableContract::offset_type offset) const
		{
			return m_innerValue.At<ReturnType>(offset);
		}

		auto At(IterableContract::offset_type offset) const
		{
			return m_innerValue.At(offset);
		}

		template<typename Type>
		void Emplace(IterableContract::offset_type offset, Type&& value)
		{
			return m_innerValue.Emplace(offset, std::forward<Type>(value));
		}
	};

	template<typename Type, typename = typename std::enable_if<IsValueContractCompliable<Type>::value>::type>
	using ProxySelector =
		typename std::conditional_t<IsValueIterable_v<Type>, IterableValueProxy<Type>,
		typename std::conditional_t<IsValueMultiOrdinal_v<Type>, MultiOrderValueProxy<Type>, SingularValueProxy<Type>
		>>;

	// Initialize the value category with corresponing type reference.
	template<typename ProxyType>
	auto ProxyInit(const Typedef::TypeFacade& typeLink, const ProxyType* ptr)
	{
		ptr->get()->ReferenceType(std::addressof(typeLink));
		ptr->get()->ValueInit();
	}

	template<typename ValueType, typename... ArgTypes>
	auto MakeValueProxy(const Typedef::TypeFacade& typeLink, ValueType&& valueCategory, ArgTypes&&... args)
	{
		auto ptr = std::make_unique<ProxySelector<ValueType>>(std::forward<ValueType>(valueCategory), std::forward<ArgTypes>(args)...);
		ProxyInit(typeLink, std::addressof(ptr));
		return ptr;
	}

	template<typename ValueType>
	auto MakeValueProxy(const Typedef::TypeFacade& typeLink)
	{
		auto ptr = std::make_unique<ProxySelector<ValueType>>();
		ProxyInit(typeLink, std::addressof(ptr));
		return ptr;
	}

public:
	// Initialize with type facade only, set value empty.
	Value(Typedef::TypeFacade&& type);
	// Initialize with base type only, set value empty.
	Value(Typedef::TypeFacade::base_type&& type);

	// Initialize with type facade and base value.
	template<typename ValueType, typename... ArgTypes>
	explicit Value(Typedef::TypeFacade&& type, ValueType&& valueCategory, ArgTypes&&... args)
		: m_internalType{ std::move(type) }
		, m_valuePtr{ MakeValueProxy<ValueType>(m_internalType, std::forward<ValueType>(valueCategory), std::forward<ArgTypes>(args)...) }
	{
	}

	// Initialize with base type and base value.
	template<typename ValueType, typename... ArgTypes>
	explicit Value(Typedef::TypeFacade::base_type&& type, ValueType&& valueCategory, ArgTypes&&... args)
		: m_internalType{ Typedef::TypeFacade{ std::move(type) } }
		, m_valuePtr{ MakeValueProxy<ValueType>(m_internalType, std::forward<ValueType>(valueCategory), std::forward<ArgTypes>(args)...) }
	{
	}

	Value(const Value&);
	Value(Value&&) = default;

	Value& operator=(const Value&);
	Value& operator=(Value&&);

	// Access type information.
	Typedef::TypeFacade Type() const { return m_internalType; }

	// Check if value is set.
	inline bool Initialized() const noexcept { return m_valuePtr->IsInitialized(); }
	// Inner value identifier.
	inline int Identifier() const { return m_valuePtr->Id(); }

	// Cast to value category type and forward inner cast to value category.
	template<typename ValueType, typename ReturnType, typename... CastTypes>
	auto As() const -> decltype(auto)
	{
		static_assert(IsValueMultiOrdinal_v<ValueType>, "value type is not multiordinal");
		using InnerProxyType = typename std::add_pointer<ProxySelector<ValueType>>::type;
		auto proxy = dynamic_cast<InnerProxyType>(m_valuePtr.get());
		if (!proxy) {
			throw InvalidTypeCastException{};
		}
		return proxy->As<ReturnType, CastTypes...>();
	}

	// Cast to value category type. //TODO: NOPE
	template<typename ValueType>
	auto As() const -> decltype(auto)
	{
		using InnerProxyType = typename std::add_pointer<ProxySelector<ValueType>>::type;
		return dynamic_cast<InnerProxyType>(m_valuePtr.get());
	}

	// Request if element vector is empty.
	template<typename ValueType>
	inline auto ElementEmpty() const
	{
		static_assert(IsValueIterable_v<ValueType>, "value type is not iterable");
		using InnerProxyType = typename std::add_pointer<ProxySelector<ValueType>>::type;
		return dynamic_cast<InnerProxyType>(m_valuePtr.get())->Empty();
	}

	// Request element vector size.
	template<typename ValueType>
	inline auto ElementCount() const
	{
		static_assert(IsValueIterable_v<ValueType>, "value type is not iterable");
		using InnerProxyType = typename std::add_pointer<ProxySelector<ValueType>>::type;
		return dynamic_cast<InnerProxyType>(m_valuePtr.get())->Size();
	}

	// Request value at offset.
	template<typename ValueType, typename ReturnType, auto Offset>
	inline auto At() const -> decltype(auto)
	{
		static_assert(IsValueIterable_v<ValueType>, "value type is not iterable");
		using InnerProxyType = typename std::add_pointer<ProxySelector<ValueType>>::type;
		return dynamic_cast<InnerProxyType>(m_valuePtr.get())->At<ReturnType>(Offset);
	}
	// Request value at offset.
	template<typename ValueType, typename ReturnType, typename SizeType>
	inline auto At(SizeType offset) const -> decltype(auto)
	{
		static_assert(IsValueIterable_v<ValueType>, "value type is not iterable");
		using InnerProxyType = typename std::add_pointer<ProxySelector<ValueType>>::type;
		return dynamic_cast<InnerProxyType>(m_valuePtr.get())->At<ReturnType>(offset);
	}

	// Replace value at offset.
	template<typename ValueType, auto Offset, typename Type>
	inline void Emplace(Type&& value) const
	{
		static_assert(IsValueIterable_v<ValueType>, "value type is not iterable");
		using InnerProxyType = typename std::add_pointer<ProxySelector<ValueType>>::type;
		dynamic_cast<InnerProxyType>(m_valuePtr.get())->Emplace(Offset, std::forward<Type>(value));
	}
	// Replace value at offset.
	template<typename ValueType, typename SizeType, typename Type>
	inline void Emplace(SizeType offset, Type&& value) const
	{
		static_assert(IsValueIterable_v<ValueType>, "value type is not iterable");
		using InnerProxyType = typename std::add_pointer<ProxySelector<ValueType>>::type;
		dynamic_cast<InnerProxyType>(m_valuePtr.get())->Emplace(offset, std::forward<Type>(value));
	}

	// Return value as string.
	const std::string ToString() const noexcept;

	// Swap this with another value.
	void Swap(Value&) noexcept;
	// Swap this with another value.
	void Swap(Value&&) noexcept;

	// Serialize the value into byte array.
	static void Serialize(const Value&, Cry::ByteArray&);
	// Serialize byte array into value.
	static void Deserialize(Value&, Cry::ByteArray&);

	// Comparison equal operator.
	bool operator==(const Value&) const;
	// Comparison not equal operator.
	bool operator!=(const Value&) const;

	Value& operator++();
	Value& operator--();
	Value operator++(int);
	Value operator--(int);

	friend Value operator+(const Value&, const Value&);
	friend Value operator-(const Value&, const Value&);
	friend Value operator*(const Value&, const Value&);
	friend Value operator/(const Value&, const Value&);
	friend Value operator%(const Value&, const Value&);

	// Check if an value was set.
	inline operator bool() const { return Initialized(); }
	// Check if an value was set.
	inline bool operator!() const { return !Initialized(); }

	// Stream value to ostream.
	friend std::ostream& operator<<(std::ostream&, const Value&);

private:
	// Value type front.
	Typedef::TypeFacade m_internalType; //TODO: replace by InternalBaseType
	// Polymorphic value interface adopting the value category.
	std::unique_ptr<ProxyInterface> m_valuePtr;
};

static_assert(std::is_copy_constructible_v<Value>, "Value !is_copy_constructible");
static_assert(std::is_move_constructible_v<Value>, "Value !is_move_constructible");
static_assert(std::is_copy_assignable_v<Value>, "Value !is_copy_assignable");
static_assert(std::is_move_assignable_v<Value>, "Value !is_move_assignable");

} // namespace CryCC::SubValue::Valuedef

#ifdef _OBSOLETE_

namespace Util
{

using namespace CryCC::SubValue::Valuedef;

//TODO: Obsolete?
struct ValueFactory
{
	static Value MakeValue(Cry::ByteArray&);
};

} // namespace Util

#endif // _OBSOLETE_
