// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <CryCC/SubValue/TypeFacade.h>
#include <CryCC/SubValue/Valuedef.h>

namespace CryCC
{
namespace SubValue
{
namespace Valuedef
{
namespace Detail
{

using namespace Typedef;

struct ValueDeductor
{
	template<BuiltinType::Specifier Specifier, typename NativeRawType>
	Valuedef::Value MakeValue(NativeRawType value)
	{
		return Valuedef::Value{TypeFacade{ Util::MakeBuiltinType(Specifier) }
			, Valuedef::Value::ValueVariantSingle{ value } };
	}

	template<BuiltinType::Specifier Specifier, typename NativeRawType>
	Valuedef::Value MakeMultiValue(NativeRawType&& value)
	{
		return Valuedef::Value{TypeFacade{ Util::MakeBuiltinType(Specifier) }
			, Valuedef::Value::ValueVariantMulti{ std::move(value) }, value.size() };
	}

	template<typename PlainType>
	Valuedef::Value ConvertNativeType(PlainType value);

	template<typename NativeType>
	void DeduceTypeQualifier(Valuedef::Value& internalValue, NativeType&&)
	{
		if (std::is_const<NativeType>::value) {
			internalValue.Type()->SetQualifier(TypedefBase::TypeQualifier::CONST_T);
		}
		if (std::is_volatile<NativeType>::value) {
			internalValue.Type()->SetQualifier(TypedefBase::TypeQualifier::VOLATILE);
		}
	}

public:
	template<typename NativeType>
	using RawType = typename std::decay<NativeType>::type;

	template<typename NativeType, typename = typename std::enable_if<Valuedef::Trait::IsAllowedType<NativeType>::value>::type>
	auto operator()(NativeType&& value)
	{
		Valuedef::Value internalValue = ConvertNativeType<RawType<NativeType>>(const_cast<RawType<NativeType>&>(value));
		DeduceTypeQualifier(internalValue, std::forward<NativeType>(value));
		return internalValue;
	}
};

//
// Value capture.
//

template<>
inline Valuedef::Value ValueDeductor::ConvertNativeType(int value)
{
	return MakeValue<BuiltinType::Specifier::INT>(value);
}
template<>
inline Valuedef::Value ValueDeductor::ConvertNativeType(char value)
{
	return MakeValue<BuiltinType::Specifier::CHAR>(value);
}
template<>
inline Valuedef::Value ValueDeductor::ConvertNativeType(float value)
{
	return MakeValue<BuiltinType::Specifier::FLOAT>(value);
}
template<>
inline Valuedef::Value ValueDeductor::ConvertNativeType(double value)
{
	return MakeValue<BuiltinType::Specifier::DOUBLE>(value);
}
template<>
inline Valuedef::Value ValueDeductor::ConvertNativeType(bool value)
{
	return MakeValue<BuiltinType::Specifier::BOOL>(value);
}
//TODO: FIXME:
//template<>
//inline Valuedef::Value ValueDeductor::ConvertNativeType(Valuedef::Value value)
//{
//	return Valuedef::Value{ TypeFacade{ Util::MakePointerType(value.Type()) }, std::move(value) };
//}

//
// MultiValue capture.
//

template<>
inline Valuedef::Value ValueDeductor::ConvertNativeType(std::vector<int> value)
{
	return MakeMultiValue<BuiltinType::Specifier::INT>(value);
}
template<>
inline Valuedef::Value ValueDeductor::ConvertNativeType(std::vector<float> value)
{
	return MakeMultiValue<BuiltinType::Specifier::FLOAT>(value);
}
template<>
inline Valuedef::Value ValueDeductor::ConvertNativeType(std::vector<double> value)
{
	return MakeMultiValue<BuiltinType::Specifier::DOUBLE>(value);
}
template<>
inline Valuedef::Value ValueDeductor::ConvertNativeType(std::vector<bool> value)
{
	return MakeMultiValue<BuiltinType::Specifier::BOOL>(value);
}

} // namespace Detail
} // namespace Valuedef
} // namespace SubValue
} // namespace CryCC
