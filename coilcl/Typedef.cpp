#include "Typedef.h"

namespace CoilCl
{
namespace Typedef
{

const std::string TypedefBase::StorageClassName() const
{
	switch (m_storageClass) {
	case StorageClassSpecifier::AUTO:		return "auto";
	case StorageClassSpecifier::STATIC:		return "static";
	case StorageClassSpecifier::EXTERN:		return "extern";
	case StorageClassSpecifier::REGISTER:	return "register";
	default:								return "";
	}
}

const std::string TypedefBase::QualifierName() const
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

const std::string BuiltinType::TypeName() const
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

} // namespace Typedef
} // namespace CoilCl