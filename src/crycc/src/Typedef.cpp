// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <CryCC/SubValue/Typedef.h>
#include <CryCC/SubValue/ArrayType.h>
#include <CryCC/SubValue/NilType.h>

namespace CryCC
{
namespace SubValue
{
namespace Typedef
{

// FUTURE: not portable, use byte stream helper
TypedefBase::buffer_type TypedefBase::TypeEnvelope() const
{
	buffer_type buffer;

	// Typedef base generic options.
	buffer.push_back(static_cast<uint8_t>(m_isInline));
	buffer.push_back(static_cast<uint8_t>(m_isSensitive));
	buffer.push_back(static_cast<uint8_t>(m_storageClass));
	buffer.push_back(static_cast<uint8_t>(m_typeQualifier[0]));
	buffer.push_back(static_cast<uint8_t>(m_typeQualifier[1]));
	return buffer;
}

const std::string TypedefBase::StorageClassName() const
{
	switch (m_storageClass) {
	case StorageClassSpecifier::AUTO:
		return "auto";
	case StorageClassSpecifier::STATIC:
		return "static";
	case StorageClassSpecifier::EXTERN:
		return "extern";
	case StorageClassSpecifier::REGISTER:
		return "register";
	default:
		return "";
	}
}

const std::string TypedefBase::QualifierName() const
{
	std::string result;
	for (const auto& qualifier : m_typeQualifier) {
		switch (qualifier) {
		case TypeQualifier::CONST_T:
			result += "const ";
			break;
		case TypeQualifier::VOLATILE:
			result += "volatile ";
			break;
		}
	}

	return result;
}

} // namespace Typedef
} // namespace SubValue
} // namespace CryCC

namespace Util
{

using namespace CryCC::SubValue::Typedef;

//TODO: Refactor, not portable
//TODO: This needs more strucutre
//TODO: THIS IS ONE F*CKED UP MESS!
BaseType MakeType(std::vector<uint8_t>&& in)
{
	assert(in.size() > 0);
	size_t envelopeOffset = 0;
	std::shared_ptr<TypedefBase> type;
	switch (static_cast<TypedefBase::TypeVariation>(in.at(0)))
	{
	case TypedefBase::TypeVariation::BUILTIN: {
		assert(in.size() > 1);
		const auto spec = static_cast<BuiltinType::Specifier>(in.at(1));
		envelopeOffset = 2;
		type = std::make_shared<BuiltinType>(spec);
		break;
	}
	case TypedefBase::TypeVariation::RECORD: {
		assert(in.size() > 2);
		std::string name;
		const auto nameSize = static_cast<size_t>(in.at(1));
		name.resize(nameSize);
		CRY_MEMCPY(static_cast<void*>(&(name[0])), name.size(), &(in.at(2)), nameSize);
		RecordType::Specifier specifier = static_cast<RecordType::Specifier>(in.at(2 + nameSize));
		//TODO: envelopeOffset = 99;
		type = std::make_shared<RecordType>(name, specifier);
		break;
	}
	case TypedefBase::TypeVariation::TYPEDEF: {
		assert(in.size() > 2);
		std::string name;
		const auto nameSize = static_cast<size_t>(in.at(1));
		name.resize(nameSize);
		CRY_MEMCPY(static_cast<void*>(&(name[0])), name.size(), &(in.at(2)), nameSize);
		//TODO: m_resolveType
		//TODO: envelopeOffset = 99;
		std::shared_ptr<TypedefBase> q = std::make_shared<VariadicType>();
		type = std::make_shared<TypedefType>(name, q);
		break;
	}
	case TypedefBase::TypeVariation::VARIADIC: {
		type = std::make_shared<VariadicType>();
		break;
	}
	case TypedefBase::TypeVariation::POINTER: {
		//TODO:
		//type = std::make_shared<PointerType>();
		break;
	}
	case TypedefBase::TypeVariation::ARRAY: {
		type = std::make_shared<ArrayType>(1, std::make_shared<NilType>());
		break;
	}
	case TypedefBase::TypeVariation::VARIANT: {
		//TODO:
		//type = std::make_shared<VariantType>();
		break;
	}
	case TypedefBase::TypeVariation::NIL: {
		type = std::make_shared<NilType>();
		break;
	}
	case TypedefBase::TypeVariation::INVAL:
	default:
		CryImplExcept(); //TODO:
	}

	{
		assert(type);
		if (static_cast<bool>(in.at(envelopeOffset))) {
			type->SetInline();
		}
		if (static_cast<bool>(in.at(envelopeOffset + 1))) {
			type->SetSensitive();
		}

		auto spec = static_cast<TypedefBase::StorageClassSpecifier>(in.at(envelopeOffset + 2));
		type->SetStorageClass(spec);
		type->SetQualifier(static_cast<TypedefBase::TypeQualifier>(in.at(envelopeOffset + 3)));
		type->SetQualifier(static_cast<TypedefBase::TypeQualifier>(in.at(envelopeOffset + 4)));
	}

	return type;
}

} // namespace Util
