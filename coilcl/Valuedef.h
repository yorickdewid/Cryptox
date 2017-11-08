#pragma once

#include "Typedef.h"

#include <boost/any.hpp>
#include <boost/lexical_cast.hpp>

#include <string>

namespace CoilCl
{
namespace Valuedef
{

class Value
{
public:
	using variant_type = boost::any;
	using array_type = std::vector<variant_type>;

protected:
	// The internal datastructure stores the value
	// as close to the actual data type specifier.
	variant_type m_value;

	// If this counter is greater than 0, the external type is an array
	struct
	{
		array_type m_Ptr;
		size_t m_Size = 0;
		bool _0terminator = false;
	} m_array;

	bool m_isVoid = false;

public:
	// Special member funcion, copy constructor
	Value(const Value& other)
		: m_objectType{ other.m_objectType }
		, m_value{ other.m_value }
		, m_isVoid{ other.m_isVoid }
		, m_isInline{ other.m_isInline }
	{
		m_array.m_Ptr = other.m_array.m_Ptr;
		m_array.m_Size = other.m_array.m_Size;
	}

	Value(std::shared_ptr<Typedef::TypedefBase> typeBase, variant_type value)
		: m_objectType{ typeBase }
		, m_value{ value }
	{
	}

	Value(std::shared_ptr<Typedef::TypedefBase> typeBase)
		: m_objectType{ typeBase }
	{
	}

	// Value class is abstract
	virtual ~Value() = default;

	// Type specifier inputs
	inline void SetInline() { m_isInline = true; }

	// Return the type specifier
	auto DataType() const { return m_objectType; }

	template<typename _CastTy>
	auto DataType() const { return std::dynamic_pointer_cast<_CastTy>(m_objectType); }

	// Check if current storage type is array
	inline auto IsArray() const { return m_array.m_Size != 0; }
	inline auto Size() const { return m_array.m_Size; }

	template<typename _Ty>
	_Ty As() const { return boost::any_cast<_Ty>(m_value); }

	virtual const std::string Print() const = 0;

private:
	bool m_isInline = false;
	std::shared_ptr<Typedef::TypedefBase> m_objectType;
};

template<typename _Ty, typename _ = void>
class ValueObject : public Value
{
	using _Myty = ValueObject<_Ty>;

public:
	ValueObject(Typedef::BuiltinType&& type, _Ty value)
		: Value{ std::make_shared<Typedef::BuiltinType>(type) }
	{
		static_assert(false, "unsupported template specialization");
	}

	virtual const std::string Print() const override
	{
		static_assert(false, "unsupported template specialization");
	}
};

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
class ValueObject < _Ty,
	typename std::enable_if < std::is_compound<_Ty>::value
	&& !std::is_void<_Ty>::value && !std::is_enum<_Ty>::value
	&& !std::is_null_pointer<_Ty>::value && !std::is_function<_Ty>::value>::type>
	: public Value
{
	using _Myty = ValueObject<_Ty>;

public:
	// Expect a string type thingy
	explicit ValueObject(Typedef::BuiltinType&& type, _Ty value)
		: Value{ std::make_shared<Typedef::BuiltinType>(type) }
	{
		m_array.m_Ptr.reserve(value.size() + 1);

		for (size_t i = 0; i < value.size(); ++i) {
			m_array.m_Ptr.push_back(value[i]);
		}

		m_array.m_Ptr.push_back('\0');
		m_array.m_Size = m_array.m_Ptr.size();
		m_array._0terminator = true;
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

} // namespace Typedef
} // namespace CoilCl
