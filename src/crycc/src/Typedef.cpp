// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <CryCC/SubValue/Typedef.h>

namespace CryCC::SubValue::Typedef
{

AbstractType::buffer_type& operator<<(AbstractType::buffer_type& os, const TypeVariation& type)
{
	os << static_cast<Cry::Byte>(type);
	return os;
}

AbstractType::buffer_type& operator>>(AbstractType::buffer_type& os, TypeVariation& type)
{
	type = TypeVariation::INVAL;
	os >> reinterpret_cast<Cry::Byte&>(type);
	return os;
}

AbstractType::buffer_type& operator<<(AbstractType::buffer_type& os, const AbstractType::TypeQualifier& qualifier)
{
	os << static_cast<Cry::Byte>(qualifier);
	return os;
}

AbstractType::buffer_type& operator>>(AbstractType::buffer_type& os, AbstractType::TypeQualifier& qualifier)
{
	qualifier = AbstractType::TypeQualifier::NONE_T;
	os >> reinterpret_cast<Cry::Byte&>(qualifier);
	return os;
}

AbstractType::buffer_type& operator<<(AbstractType::buffer_type& os, const AbstractType::StorageClassSpecifier& specifier)
{
	os << static_cast<Cry::Byte>(specifier);
	return os;
}

AbstractType::buffer_type& operator>>(AbstractType::buffer_type& os, AbstractType::StorageClassSpecifier& specifier)
{
	specifier = AbstractType::StorageClassSpecifier::NONE_T;
	os >> reinterpret_cast<Cry::Byte&>(specifier);
	return os;
}

// Pack type into reserved buffer. Call the packer method which can be overridden
// by the derived type. This call will at least pack the abstract type properties.
void AbstractType::Serialize(const AbstractType& type, buffer_type& buffer)
{
	type.Pack(buffer);
}

// Unpack type from reserved buffer. Call the unpacker method which can be overridden
// by the derived type. This call will at least unpack the abstract type properties.
void AbstractType::Deserialize(AbstractType& type, buffer_type& buffer)
{
	type.Unpack(buffer);
}

// Store generic type properties into buffer.
// This method should only be called from derived types.
//
// The type identifier is used only for the deserialization
// phase or any other type testing operation. The stream barrier
// signals the beginning of the abstract type properties. This
// makes it easiers to remove the type identifier if not done already.
void AbstractType::Pack(buffer_type& buffer) const
{
	buffer << TypeId();
	buffer << buffer_type::StreamBarrier;
	buffer << m_isInline;
	buffer << m_isSensitive;
	buffer << m_storageClass;
	buffer << m_typeQualifier;
}

// Retrieve generic type properties from buffer.
// This method should only be called from derived types.
void AbstractType::Unpack(buffer_type& buffer)
{
	// If no stream barrier was found, remove the type identifier. 
	if (!buffer.HasStreamBarrier()) {
		buffer >> Cry::ByteStream::Ignore<buffer_type, TypeVariation>{};
		if (!buffer.HasStreamBarrier()) {
			CryImplExcept(); // TODO: 
		}
	}

	buffer >> m_isInline;
	buffer >> m_isSensitive;
	buffer >> m_storageClass;
	buffer >> m_typeQualifier;
}

const std::string AbstractType::StorageClassName() const
{
	switch (m_storageClass) {
	case StorageClassSpecifier::AUTO_T:
		return "auto";
	case StorageClassSpecifier::STATIC_T:
		return "static";
	case StorageClassSpecifier::EXTERN_T:
		return "extern";
	case StorageClassSpecifier::REGISTER_T:
		return "register";
	}

	return {};
}

const std::string AbstractType::QualifierName() const
{
	std::string result;
	for (const auto& qualifier : m_typeQualifier) {
		switch (qualifier) {
		case TypeQualifier::CONST_T:
			result += "const ";
			break;
		case TypeQualifier::VOLATILE_T:
			result += "volatile ";
			break;
		}
	}

	return result;
}

} // namespace CryCC::SubValue::Typedef

#if _OBSOLETE_
namespace Util
{

using namespace CryCC::SubValue::Typedef;

//TODO: Refactor, not portable
//TODO: This needs more strucutre
//TODO: THIS IS ONE F*CKED UP MESS!
BaseType MakeType(std::vector<uint8_t>&& in)
{
	return nullptr;
	assert(in.size() > 0);
	size_t envelopeOffset = 0;
	std::shared_ptr<AbstractType> type;
	switch (static_cast<AbstractType::TypeVariation>(in.at(0)))
	{
	case AbstractType::TypeVariation::BUILTIN: {
		assert(in.size() > 1);
		const auto spec = static_cast<BuiltinType::Specifier>(in.at(1));
		envelopeOffset = 2;
		type = std::make_shared<BuiltinType>(spec);
		break;
	}
	case AbstractType::TypeVariation::RECORD: {
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
	case AbstractType::TypeVariation::TYPEDEF: {
		assert(in.size() > 2);
		std::string name;
		const auto nameSize = static_cast<size_t>(in.at(1));
		name.resize(nameSize);
		CRY_MEMCPY(static_cast<void*>(&(name[0])), name.size(), &(in.at(2)), nameSize);
		//TODO: m_resolveType
		//TODO: envelopeOffset = 99;
		std::shared_ptr<AbstractType> q = std::make_shared<VariadicType>();
		type = std::make_shared<TypedefType>(name, q);
		break;
	}
	case AbstractType::TypeVariation::VARIADIC: {
		type = std::make_shared<VariadicType>();
		break;
	}
	case AbstractType::TypeVariation::POINTER: {
		//TODO:
		//type = std::make_shared<PointerType>();
		break;
	}
	case AbstractType::TypeVariation::ARRAY: {
		type = std::make_shared<ArrayType>(1, std::make_shared<NilType>());
		break;
	}
	case AbstractType::TypeVariation::VARIANT: {
		//TODO:
		//type = std::make_shared<VariantType>();
		break;
	}
	case AbstractType::TypeVariation::NIL: {
		type = std::make_shared<NilType>();
		break;
	}
	case AbstractType::TypeVariation::INVAL:
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

		auto spec = static_cast<AbstractType::StorageClassSpecifier>(in.at(envelopeOffset + 2));
		type->SetStorageClass(spec);
		type->SetQualifier(static_cast<AbstractType::TypeQualifier>(in.at(envelopeOffset + 3)));
		type->SetQualifier(static_cast<AbstractType::TypeQualifier>(in.at(envelopeOffset + 4)));
	}

	return type;
}

} // namespace Util
#endif // _OBSOLETE_
