// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "UnsupportedOperationException.h"

#include <cassert>
#include <string>
#include <memory>
#include <vector>
#include <list>
#include <bitset>

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
	virtual bool AllowCoalescence() const = 0;
	virtual void Consolidate(std::shared_ptr<TypedefBase>& type) = 0;
	virtual size_t UnboxedSize() const = 0;
	virtual bool Equals(TypedefBase* other) const = 0;

	// Type specifier inputs
	inline void SetStorageClass(StorageClassSpecifier storageClass) { m_storageClass = storageClass; }
	inline void SetQualifier(TypeQualifier qypeQualifier) { m_typeQualifier.push_back(qypeQualifier); }
	inline void SetInline() { m_isInline = true; }

	// Stringify type name
	const std::string StorageClassName() const;
	const std::string QualifierName() const;

	// Additional type specifiers
	inline StorageClassSpecifier StorageClass() const { return m_storageClass; }
	inline auto IsInline() const { return m_isInline; }

protected:
	bool m_isInline = false;
	StorageClassSpecifier m_storageClass;
	std::vector<TypeQualifier> m_typeQualifier{ 2 };
};

class BuiltinType : public TypedefBase
{
	// Additional type options
	enum
	{
		IS_SIGNED,
		IS_UNSIGNED,
		IS_SHORT,
		IS_LONG,
		IS_LONG_LONG,
		IS_COMPLEX,
		IS_IMAGINARY,
	};

	std::bitset<8> m_typeOptions;

private:
	// If specifier matches a type option, set the option bit
	// and default the type to integer.
	void SpecifierToOptions()
	{
		switch (m_specifier) {
		case Specifier::SIGNED:
			m_typeOptions.set(IS_SIGNED);
			m_typeOptions.reset(IS_UNSIGNED);
			m_specifier = Specifier::INT;
			break;
		case Specifier::UNSIGNED:
			m_typeOptions.set(IS_UNSIGNED);
			m_typeOptions.set(IS_SIGNED);
			m_specifier = Specifier::INT;
			break;
		case Specifier::SHORT:
			m_typeOptions.set(IS_SHORT);
			m_specifier = Specifier::INT;
			break;
		case Specifier::LONG:
			m_typeOptions.set(IS_LONG);
			m_specifier = Specifier::INT;
			break;
		}
	}

public:
	enum class Specifier
	{
		VOID,
		CHAR,
		SHORT,
		INT,
		LONG,
		SIGNED,
		UNSIGNED,
		FLOAT,
		DOUBLE,
		BOOL,
	};

public:
	BuiltinType(Specifier specifier)
		: m_specifier{ specifier }
	{
		SpecifierToOptions();
	}

	// Set type options
	inline auto Unsigned() const { return m_typeOptions.test(IS_UNSIGNED); }
	inline auto Signed() const { return !Unsigned(); }
	inline auto Short() const { return m_typeOptions.test(IS_SHORT); }
	inline auto Long() const { return m_typeOptions.test(IS_LONG); }
	inline auto Complex() const { return m_typeOptions.test(IS_COMPLEX); }
	inline auto Imaginary() const { return m_typeOptions.test(IS_IMAGINARY); }

	// Return type name string
	const std::string TypeName() const;

	// If any type options are set, allow type coalescence
	bool AllowCoalescence() const { return m_typeOptions.any(); }

	auto TypeSpecifier() const { return m_specifier; }

	size_t UnboxedSize() const;

	bool Equals(TypedefBase* other) const
	{
		auto self = dynamic_cast<BuiltinType*>(other);
		if (self == nullptr) {
			return false;
		}

		return m_specifier == self->m_specifier
			&& m_typeOptions == self->m_typeOptions;
	}

	void Consolidate(std::shared_ptr<TypedefBase>& type)
	{
		assert(type->AllowCoalescence());

		auto otherType = std::dynamic_pointer_cast<BuiltinType>(type);
		if (otherType->Unsigned()) { m_typeOptions.set(IS_UNSIGNED); }
		if (otherType->Short()) { m_typeOptions.set(IS_SHORT); }
		if (otherType->Long()) {
			if (this->Long()) {
				m_typeOptions.set(IS_LONG_LONG);
			}
			else {
				m_typeOptions.set(IS_LONG);
			}
		}
	}

private:
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

	const std::string TypeName() const final
	{
		return (m_specifier == Specifier::UNION ? "union " : "struct ") + m_name;
	}

	bool AllowCoalescence() const final { return false; }

	//TODO: quite the puzzle
	size_t UnboxedSize() const { return 0; }

	bool Equals(TypedefBase* other) const
	{
		auto self = dynamic_cast<RecordType*>(other);
		if (self == nullptr) {
			return false;
		}

		return m_specifier == self->m_specifier
			&& m_name == self->m_name;
	}

	void Consolidate(std::shared_ptr<TypedefBase>& type)
	{
		throw UnsupportedOperationException{ "TypedefType::Consolidate" };
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

	const std::string TypeName() const final
	{
		return m_name + ":" + m_resolveType->TypeName();
	}

	bool AllowCoalescence() const final { return false; }

	size_t UnboxedSize() const { return m_resolveType->UnboxedSize(); }

	bool Equals(TypedefBase* other) const
	{
		auto self = dynamic_cast<TypedefType*>(other);
		if (self == nullptr) {
			return false;
		}

		return m_resolveType == self->m_resolveType
			&& m_name == self->m_name;
	}

	void Consolidate(std::shared_ptr<TypedefBase>& type)
	{
		throw UnsupportedOperationException{ "TypedefType::Consolidate" };
	}
};

class VariadicType : public TypedefBase
{
public:
	const std::string TypeName() const final { return "..."; }
	bool AllowCoalescence() const final { return false; }
	size_t UnboxedSize() const { return 0; }

	bool Equals(TypedefBase* other) const
	{
		return dynamic_cast<VariadicType*>(other) != nullptr;
	}

	void Consolidate(std::shared_ptr<TypedefBase>& type)
	{
		throw UnsupportedOperationException{ "VariadicType::Consolidate" };
	}
};

} // namespace Typedef

namespace Util
{

template<typename... _TyArgs>
inline auto MakeBuiltinType(_TyArgs&&... args)
{
	return std::make_shared<Typedef::BuiltinType>(std::forward<_TyArgs>(args)...);
}

template<typename... _TyArgs>
inline auto MakeRecordType(const std::string& name, _TyArgs&&... args)
{
	return std::make_shared<Typedef::RecordType>(name, args...);
}

inline auto MakeTypedefType(const std::string& name, std::shared_ptr<Typedef::TypedefBase>& type)
{
	return std::make_shared<Typedef::TypedefType>(name, type);
}

inline auto MakeVariadicType()
{
	return std::make_shared<Typedef::VariadicType>();
}

} // namespace Util
} // namespace CoilCl
