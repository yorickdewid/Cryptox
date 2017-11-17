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

	std::string option;
	if (m_typeOptions.test(IS_UNSIGNED)) {
		option += "unsigned ";
	}
	if (m_typeOptions.test(IS_SHORT)) {
		option += "short ";
	}
	if (m_typeOptions.test(IS_LONG_LONG)) {
		option += "long long ";
	}
	else if (m_typeOptions.test(IS_LONG)) {
		option += "long ";
	}

	switch (m_specifier) {
	case Specifier::VOID:	qualifier += option + "void"; break;
	case Specifier::CHAR:	qualifier += option + "char"; break;
	case Specifier::LONG:	qualifier += option + "long"; break;
	case Specifier::SHORT:	qualifier += option + "short"; break;
	case Specifier::INT:	qualifier += option + "int"; break;
	case Specifier::FLOAT:	qualifier += option + "float"; break;
	case Specifier::DOUBLE:	qualifier += option + "double"; break;
	case Specifier::BOOL:	qualifier += option + "bool"; break;
	default:				qualifier += option + "<unknown>"; break;
	}

	return qualifier;
}

} // namespace Typedef
} // namespace CoilCl
