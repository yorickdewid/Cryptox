#pragma once

#include <string>
#include <memory>

namespace CoilCl
{
namespace Typedef
{

class TypedefBase
{
public:
	virtual const std::string TypeName() const = 0;
};

class BuiltinType : public TypedefBase
{
public:
	enum class Specifier
	{
		VOID,
		CHAR,
		SHORT,
		INT,
		LONG,
		FLOAT,
		DOUBLE,
		BOOL,
	};

	BuiltinType(Specifier specifier)
		: m_specifier{ specifier }
	{
	}

	const std::string TypeName() const
	{
		switch (m_specifier) {
		case Specifier::VOID:	return "void";
		case Specifier::CHAR:	return "char";
		case Specifier::SHORT:	return "short";
		case Specifier::INT:	return "int";
		case Specifier::LONG:	return "long";
		case Specifier::FLOAT:	return "float";
		case Specifier::DOUBLE:	return "double";
		case Specifier::BOOL:	return "bool";
		default:				return "<unknown>";
		}
	}

private:
	Specifier m_specifier;
};

class RecordType : public TypedefBase
{
	std::unique_ptr<TypedefBase> m_upCast;

public:
	const std::string TypeName() const
	{
		return "struct xxx:struct xxx";
	}
};

class TypedefType : public TypedefBase
{
	std::unique_ptr<TypedefBase> m_upCast;

public:
	const std::string TypeName() const
	{
		return "woeit_t:int";
	}
};

} // namespace Typedef

namespace Util
{

inline auto MakeBuiltinType(Typedef::BuiltinType::Specifier specifier)
{
	return std::make_unique<Typedef::BuiltinType>(specifier);
}

inline auto MakeRecordType()
{
	return std::make_unique<Typedef::RecordType>();
}

inline auto MakeTypedefType()
{
	return std::make_unique<Typedef::TypedefType>();
}

} // namespace Util
} // namespace CoilCl
