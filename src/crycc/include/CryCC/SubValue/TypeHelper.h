// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <CryCC/SubValue/Typedef.h>
#include <CryCC/SubValue/BuiltinType.h>
#include <CryCC/SubValue/RecordType.h>
#include <CryCC/SubValue/TypedefType.h>
#include <CryCC/SubValue/VariadicType.h>
#include <CryCC/SubValue/ArrayType.h>
#include <CryCC/SubValue/VariantType.h>
#include <CryCC/SubValue/NilType.h>
#include <CryCC/SubValue/PointerType.h>
#include <CryCC/SubValue/TypeFacade.h>

// The type helper functions ease the creation and modification of the
// type objects as well as some functions to query specific properties.
// In any case should the helper functions be used instead of accessing
// the type objects directly. The helper functions are designed to support
// the most common types as such that the caller does not need to cast the type.
namespace Util
{

using namespace CryCC::SubValue::Typedef;

BaseType MakeType(std::vector<uint8_t>&&); // TODO: OBSOLETE?

//
// Type creation helpers.
//

InternalBaseType MakeBuiltinType(BuiltinType::Specifier specifier);
InternalBaseType MakeRecordType(const std::string& name, RecordType::Specifier specifier);
InternalBaseType MakeTypedefType(const std::string& name, InternalBaseType& type);
InternalBaseType MakeTypedefType(const std::string& name, TypeFacade type);
InternalBaseType MakeVariadicType();
InternalBaseType MakeArrayType(size_t elements, InternalBaseType& type);
InternalBaseType MakeVariantType();
InternalBaseType MakeNilType();
InternalBaseType MakePointerType(InternalBaseType& type);
InternalBaseType MakePointerType(TypeFacade type);

//
// Type query helpers.
//

//TODO:
//size_t ArrayExtend(const InternalBaseType&) noexcept;
//size_t InnateSize(const InternalBaseType&) noexcept;
//size_t PointerCount(const InternalBaseType&) noexcept;

bool IsVoid(const InternalBaseType&) noexcept;
bool IsIntegral(const InternalBaseType&) noexcept;
bool IsFloatingPoint(const InternalBaseType&) noexcept;
bool IsBool(const InternalBaseType&) noexcept;
bool IsArray(const InternalBaseType&) noexcept;
bool IsEnum(const InternalBaseType&) noexcept;
bool IsStruct(const InternalBaseType&) noexcept;
bool IsUnion(const InternalBaseType&) noexcept;
bool IsClass(const InternalBaseType&) noexcept;
bool IsRecord(const InternalBaseType&) noexcept;
bool IsPointer(const InternalBaseType&) noexcept;
bool IsInline(const InternalBaseType&) noexcept;
bool IsSensitive(const InternalBaseType&) noexcept;
bool IsStatic(const InternalBaseType&) noexcept;
bool IsExtern(const InternalBaseType&) noexcept;
bool IsRegister(const InternalBaseType&) noexcept;
bool IsConst(const InternalBaseType&) noexcept;
bool IsVolatile(const InternalBaseType&) noexcept;
bool IsSigned(const InternalBaseType&) noexcept;
bool IsUnsigned(const InternalBaseType&) noexcept;

} // namespace Util
