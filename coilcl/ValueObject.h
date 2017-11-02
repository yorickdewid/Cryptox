#pragma once

#include <boost/scoped_array.hpp>

#include <string>

struct Value
{
	// Storage type, only for external usage
	enum class TypeSpecifier
	{
		T_VOID,
		T_CHAR,
		T_SHORT,
		T_INT,
		T_LONG,
		T_FLOAT,
		T_DOUBLE,
		T_BOOL,
		T_PTR,
	};

	// Storage class specifier
	enum class StorageClassSpecifier
	{
		NONE,
		AUTO,
		STATIC,
		EXTERN,
		TYPEDEF,
		REGISTER,
	};

	// Type qualifier
	enum class TypeQualifier
	{
		NONE,
		CONST,
		VOLATILE,
	};

protected:
	// The internal datastructure stores the value
	// as close to the actual data type specifier.
	union StoreValue
	{
		int i;
		float f;
		double d;
		char c;
		intptr_t *p;
	};

	// If this counter is greater than 0, the external type is an array
	boost::scoped_array<StoreValue> m_arrayPtr;
	size_t m_arraySize = 0;

	// Value objects must implement a function
	// to return the stored value
	virtual StoreValue ReturnValue() const = 0;

	// Function inliner
	bool isInline = false;

public:
	Value(TypeSpecifier type)
		: m_objectType{ type }
	{
	}

	//TODO: Copy arrayPtr somehow
	Value(const Value& other)
		: m_isUnsigned{ other.m_isUnsigned }
		, m_objectType{ other.m_objectType }
		, m_scSpecifier{ other.m_scSpecifier }
		, m_typeQualifier{ other.m_typeQualifier }
		, m_arraySize{ other.m_arraySize }
	{
	}

	// Value class is abstract
	virtual ~Value() = default;

	// Type specifier inputs
	inline void StorageClass(StorageClassSpecifier scp) { m_scSpecifier = scp; }
	inline void Qualifier(TypeQualifier tq) { m_typeQualifier = tq; }
	inline void SetInline() { isInline = true; }

	// Return the type specifier
	TypeSpecifier DataType() const
	{
		return m_objectType;
	}

	// Check if current storage type is array
	inline auto IsArray() const
	{
		return m_arraySize != 0;
	}

	inline auto Size() const
	{
		return m_arraySize;
	}

	template<typename _Ty>
	_Ty As() const
	{
		return static_cast<_Ty>(ReturnValue().i);
	}

	template<>
	float As() const
	{
		return ReturnValue().f;
	}

	template<>
	std::string As() const
	{
		std::string str;
		str.reserve(m_arraySize);

		for (size_t i = 0; i < m_arraySize; ++i) {
			if (i == (m_arraySize - 1) && m_arrayPtr[i].c == '\0') {
				break;
			}
			str.push_back(m_arrayPtr[i].c);
		}

		return str;
	}

	std::string ToString() const
	{
		auto _type = ReturnValue();
		switch (m_objectType) {
		case Value::TypeSpecifier::T_CHAR:
			return IsArray() ? As<std::string>() : std::string{ _type.c };
		case Value::TypeSpecifier::T_SHORT:
		case Value::TypeSpecifier::T_INT:
		case Value::TypeSpecifier::T_LONG:
			return std::to_string(_type.i);
		case Value::TypeSpecifier::T_FLOAT:
			return std::to_string(_type.f);
		case Value::TypeSpecifier::T_DOUBLE:
			return std::to_string(_type.d);
		case Value::TypeSpecifier::T_BOOL:
			return static_cast<bool>(_type.i) ? "true" : "false";
		case Value::TypeSpecifier::T_PTR:
			return "<ptr>";
		default:
			return "<unknown>";
		}
	}

	friend std::ostream& operator<<(std::ostream& os, const Value& value)
	{
		os << value.ToString();
	}

private:
	bool m_isUnsigned = false;
	TypeSpecifier m_objectType = TypeSpecifier::T_INT;
	StorageClassSpecifier m_scSpecifier = StorageClassSpecifier::NONE;
	TypeQualifier m_typeQualifier = TypeQualifier::NONE;
};

template<typename _Ty>
class ValueObject : public Value
{
	StoreValue m_value;

	StoreValue ReturnValue() const override
	{
		return m_value;
	}

	typedef ValueObject<_Ty> _Myty;

public:
	ValueObject(TypeSpecifier type, _Ty v)
		: Value{ type }
		, m_value{ v }
	{
	}

	ValueObject(TypeSpecifier type)
		: Value{ type }
	{
	}

	ValueObject(const _Myty& other)
		: Value{ other }
		, m_value{ other.m_value }
	{
	}
};

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
