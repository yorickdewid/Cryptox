#pragma once

#include <string>

class Flags;

struct Value
{
	enum ObjectType
	{
		T_INT,
		T_FLOAT,
		T_DOUBLE,
		T_CHAR,
		T_STRING,
	} m_objectType = T_INT;

	enum class Flags
	{
		NONE = 1 << 0,
		CONST = 1 << 1,
		STATIC = 1 << 2,
		UNSIGNED = 1 << 3,
		REGISTER = 1 << 4,
		VOLATILE = 1 << 5,
	};

protected:
	union StoreValue
	{
		int i;
		float f;
		double d;
		char c;
		struct
		{
			char *ptr;
			size_t len;
		} str;
	};

	virtual StoreValue ReturnValue() const = 0;

public:
	Value(ObjectType type)
		: m_objectType{ type }
	{
	}

	virtual ~Value() = default;

	ObjectType DataType() const
	{
		return m_objectType;
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
		return std::string{ ReturnValue().str.ptr, ReturnValue().str.len };
	}

private:
	Flags m_flags = Flags::NONE;
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
	ValueObject(ObjectType type, Type v)
		: Value{ type }
		, m_value{ v }
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
	ValueObject(ObjectType type, float v)
		: Value{ type }
	{
		m_value.f = v;
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
	ValueObject(ObjectType type, std::string str)
		: Value{ type }
	{
		m_value.str.ptr = new char[str.size()];
		m_value.str.len = str.size();

		::memcpy_s(m_value.str.ptr, m_value.str.len, str.c_str(), str.size());
	}

	~ValueObject()
	{
		delete[] m_value.str.ptr;
	}
};
