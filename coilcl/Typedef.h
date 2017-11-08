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
	// Abstract methods
	virtual const std::string TypeName() const = 0;

	// Type specifier inputs
	inline void StorageClass(StorageClassSpecifier storageClass) { m_storageClass = storageClass; }
	inline void Qualifier(TypeQualifier qypeQualifier) { m_typeQualifier.push_back(qypeQualifier); }

	const std::string StorageClassName() const;
	const std::string QualifierName() const;

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

	// Set MSB signness
	inline void SetUnsigned() { m_isSigned = false; }
	inline auto Signed() const { return m_isSigned; }
	inline auto Unsigned() const { return !Signed(); }

	const std::string TypeName() const;

	auto TypeSpecifier() const { return m_specifier; }

private:
	bool m_isSigned = true;
	Specifier m_specifier;
};

class RecordType : public TypedefBase
{
	std::unique_ptr<TypedefBase> m_upCast;

public:
	const std::string TypeName() const { return "struct xxx:struct xxx"; }
};

class TypedefType : public TypedefBase
{
	std::unique_ptr<TypedefBase> m_upCast;

public:
	const std::string TypeName() const { return "woeit_t:int"; }
	const std::string ValueToString() const { return ""; }
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
