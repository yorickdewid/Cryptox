#pragma once

#include <string>
#include <memory>
#include <vector>

namespace CoilCl
{
namespace Typedef
{

class TypedefBase
{
public:
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

public:
	virtual const std::string TypeName() const = 0;

	// Type specifier inputs
	inline void StorageClass(StorageClassSpecifier storageClass) { m_storageClass = storageClass; }
	inline void Qualifier(TypeQualifier qypeQualifier) { m_typeQualifier.push_back(qypeQualifier); }
	//inline void SetInline() { isInline = true; }

	const std::string StorageClassName() const
	{
		switch (m_storageClass) {
		case StorageClassSpecifier::AUTO:		return "auto";
		case StorageClassSpecifier::STATIC:		return "static";
		case StorageClassSpecifier::EXTERN:		return "extern";
		case StorageClassSpecifier::REGISTER:	return "register";
		default:								return "";
		}
	}

	const std::string QualifierName() const
	{
		std::string result;
		for (const auto& qualifier : m_typeQualifier) {
			switch (qualifier) {
			case TypeQualifier::CONST:		result += "const "; break;
			case TypeQualifier::VOLATILE:	result += "volatile "; break;
			}
		}

		return result;
	}

protected:
	StorageClassSpecifier m_storageClass;
	std::vector<TypeQualifier> m_typeQualifier{ 2 };
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
		auto qualifier = TypedefBase::QualifierName();

		switch (m_specifier) {
		case Specifier::VOID:	qualifier += "void"; break;
		case Specifier::CHAR:	qualifier += "char"; break;
		case Specifier::SHORT:	qualifier += "short"; break;
		case Specifier::INT:	qualifier += "int"; break;
		case Specifier::LONG:	qualifier += "long"; break;
		case Specifier::FLOAT:	qualifier += "float"; break;
		case Specifier::DOUBLE:	qualifier += "double"; break;
		case Specifier::BOOL:	qualifier += "bool"; break;
		default:				qualifier += "<unknown>"; break;
		}

		return qualifier;
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
	return std::make_shared<Typedef::BuiltinType>(specifier);
}

inline auto MakeRecordType()
{
	return std::make_shared<Typedef::RecordType>();
}

inline auto MakeTypedefType()
{
	return std::make_shared<Typedef::TypedefType>();
}

} // namespace Util
} // namespace CoilCl
