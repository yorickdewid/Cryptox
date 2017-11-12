#pragma once

#include <string>
#include <memory>
#include <vector>
#include <list>

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
	inline void SetStorageClass(StorageClassSpecifier storageClass) { m_storageClass = storageClass; }
	inline void SetQualifier(TypeQualifier qypeQualifier) { m_typeQualifier.push_back(qypeQualifier); }

	const std::string StorageClassName() const;
	const std::string QualifierName() const;

	inline StorageClassSpecifier StorageClass() const { return m_storageClass; }

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
	std::list<std::string> m_recordList; //?

public:
	const std::string TypeName() const { return "struct xxx:struct xxx"; }
};

class TypedefType : public TypedefBase
{
	std::string m_name;
	std::shared_ptr<TypedefBase> m_resolveType;

public:
	TypedefType(const std::string& name, std::shared_ptr<TypedefBase>& nativeType)
		: m_name{ name }
		, m_resolveType{ std::move(nativeType) }
	{
	}

	const std::string TypeName() const
	{
		return m_name + ":" + m_resolveType->TypeName();
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

inline auto MakeTypedefType(const std::string& name, std::shared_ptr<Typedef::TypedefBase>& type)
{
	return std::make_shared<Typedef::TypedefType>(name, type);
}

} // namespace Util
} // namespace CoilCl
