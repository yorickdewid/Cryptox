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

public:
	Value(TypeSpecifier type)
		: m_objectType{ type }
	{
	}

	Value(const Value& other)
		: m_isUnsigned{ other.m_isUnsigned }
		, m_objectType{ other.m_objectType }
		, m_scSpecifier{ other.m_scSpecifier }
		, m_typeQualifier{ other.m_typeQualifier }
	{
	}

	virtual ~Value() = default;

	inline void StorageClass(StorageClassSpecifier scp)
	{
		m_scSpecifier = scp;
	}

	inline void Qualifier(TypeQualifier tq)
	{
		m_typeQualifier = tq;
	}

	// Return the type specifier
	TypeSpecifier DataType() const
	{
		return m_objectType;
	}

	// Check if current storage type is array
	inline bool IsArray() const
	{
		return m_arraySize != 0;
	}

	template<typename Type>
	Type As() const
	{
		return static_cast<Type>(ReturnValue().i);
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

		for (size_t i = 0; i < m_arraySize; ++i) {
			str.push_back(m_arrayPtr[i].c);
		}

		return str;
	}

private:
	bool m_isUnsigned = false;
	TypeSpecifier m_objectType = TypeSpecifier::T_INT;
	StorageClassSpecifier m_scSpecifier = StorageClassSpecifier::NONE;
	TypeQualifier m_typeQualifier = TypeQualifier::NONE;
};

template<typename Type>
class ValueObject : public Value
{
	StoreValue m_value;

	StoreValue ReturnValue() const override
	{
		return m_value;
	}

public:
	ValueObject(TypeSpecifier type, Type v)
		: Value{ type }
		, m_value{ v }
	{
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

	ValueObject(const ValueObject& other)
		: Value(other)
		, m_value{ other.m_value }
	{
	}
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
		m_arrayPtr.reset(new StoreValue[str.size()]);

		for (size_t i = 0; i < str.size(); ++i) {
			m_arrayPtr[i].c = str[i];
		}

		m_arraySize = str.size();
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
