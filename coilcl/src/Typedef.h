// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

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

public:
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
	std::string m_name;

public:
	enum class Specifier
	{
		STRUCT,
		UNION,
	};

public:
	RecordType(const std::string& name, Specifier specifier)
		: m_name{ name }
		, m_specifier{ specifier }
	{
	}

	const std::string TypeName() const
	{
		return (m_specifier == Specifier::UNION ? "union " : "struct ") + m_name;
	}

private:
	Specifier m_specifier;
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

inline auto MakeRecordType(const std::string& name, Typedef::RecordType::Specifier specifier)
{
	return std::make_shared<Typedef::RecordType>(name, specifier);
}

inline auto MakeTypedefType(const std::string& name, std::shared_ptr<Typedef::TypedefBase>& type)
{
	return std::make_shared<Typedef::TypedefType>(name, type);
}

} // namespace Util
} // namespace CoilCl
