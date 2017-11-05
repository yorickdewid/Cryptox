#pragma once

#include "Typedef.h"

#include <boost/scoped_array.hpp>
#include <boost/variant.hpp>

class Value
{
public:
	using variant_type = boost::variant<int, float, double, char, std::string>;
	using array_type = boost::scoped_array<variant_type>;

protected:
	// The internal datastructure stores the value
	// as close to the actual data type specifier.
	variant_type m_value;

	// If this counter is greater than 0, the external type is an array
	array_type m_arrayPtr;
	size_t m_arraySize = 0;

	// Function inliner
	bool isInline = false;

public:
	//TODO: Copy arrayPtr somehow
	Value(const Value& other)
		: m_isUnsigned{ other.m_isUnsigned }
		, m_objectType{ other.m_objectType }
		, m_arraySize{ other.m_arraySize }
	{
	}

	Value(std::shared_ptr<CoilCl::Typedef::TypedefBase> typeBase, variant_type value)
		: m_objectType{ typeBase }
		, m_value{ value }
	{
	}

	// Value class is abstract
	virtual ~Value() = default;

	// Type specifier inputs
	inline void SetInline() { isInline = true; }

	// Return the type specifier
	auto DataType() const { return m_objectType; }

	template<typename _CastTy>
	auto DataType() const { return std::dynamic_pointer_cast<_CastTy>(m_objectType); }

	// Check if current storage type is array
	inline auto IsArray() const { return m_arraySize != 0; }
	inline auto Size() const { return m_arraySize; }

	template<typename _Ty>
	_Ty As() const { return boost::get<_Ty>(m_value); }

	// Stream variant output
	friend std::ostream& operator<<(std::ostream& os, const Value& value)
	{
		os << value.m_value;
		return os;
	}

private:
	bool m_isUnsigned = false;
	bool m_isVoid = false;
	std::shared_ptr<CoilCl::Typedef::TypedefBase> m_objectType;
};

template<typename _Ty>
class ValueObject : public Value
{
	using _Myty = ValueObject<_Ty>;

public:
	ValueObject(CoilCl::Typedef::BuiltinType&& type, _Ty value)
		: Value{ std::make_shared<CoilCl::Typedef::BuiltinType>(type), value }
	{
	}

	/*ValueObject(CoilCl::Typedef::RecordType&& type, _Ty value)
		: Value{ std::make_shared<CoilCl::Typedef::RecordType>(type), value }
	{
	}*/

	ValueObject(const _Myty& other) = default;
	ValueObject(_Myty&& other) = default;
};

#if 0

template<>
class ValueObject<void> : public Value
{
	StoreValue m_value;

	StoreValue ReturnValue() const override
	{
		return m_value;
	}

public:
	ValueObject(TypeSpecifier type)
		: Value{ type }
	{
	}

	ValueObject(const ValueObject& other) = default;
};

template<>
class ValueObject<float> : public Value
{
	StoreValue m_value;

	StoreValue ReturnValue() const override
	{
		return m_value;
	}

public:
	ValueObject(TypeSpecifier type, float v)
		: Value{ type }
	{
		m_value.f = v;
	}

	ValueObject(TypeSpecifier type)
		: Value{ type }
	{
	}

	ValueObject(const ValueObject& other)
		: Value(other)
		, m_value{ other.m_value }
	{
	}
};

template<>
class ValueObject<double> : public Value
{
	StoreValue m_value;

	StoreValue ReturnValue() const override
	{
		return m_value;
	}

public:
	ValueObject(TypeSpecifier type, double v)
		: Value{ type }
	{
		m_value.d = v;
	}

	ValueObject(TypeSpecifier type)
		: Value{ type }
	{
	}

	ValueObject(const ValueObject& other)
		: Value(other)
		, m_value{ other.m_value }
	{
	}
};

template<>
class ValueObject<std::string> : public Value
{
	StoreValue m_value;

	StoreValue ReturnValue() const override
	{
		return m_value;
	}

public:
	ValueObject(TypeSpecifier type, std::string str)
		: Value{ type }
	{
		m_arrayPtr.reset(new StoreValue[str.size() + 1]);

		for (size_t i = 0; i < str.size(); ++i) {
			m_arrayPtr[i].c = str[i];
		}

		m_arrayPtr[str.size()].c = '\0';
		m_arraySize = str.size() + 1;
	}

	ValueObject(TypeSpecifier type)
		: Value{ type }
	{
	}

	ValueObject(const ValueObject& other)
		: Value(other)
		, m_value{ other.m_value }
	{
	}
};

#endif