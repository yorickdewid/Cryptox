// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <CryCC/SubValue/ArrayValue.h>
#include <CryCC/SubValue/Valuedef.h>
#include <CryCC/SubValue/ValueHelper.h>

#include <boost/variant.hpp>

namespace CryCC
{
namespace SubValue
{
namespace Valuedef
{

// using namespace CryCC::SubValue::Typedef;

struct ArrayAccess
{
	struct VectorVisitor final : public boost::static_visitor<>
	{
		size_t offset;

		VectorVisitor(size_t offset)
			: offset{ offset }
		{
		}

		template<typename Type>
		void operator()(const std::vector<Type>& value) const
		{
			value.at(offset);
		}
	};

	static void woei(ArrayValue& value, size_t offset)
	{
		VectorVisitor visit{ offset };
		value.m_value.apply_visitor(visit);
	}
};

std::shared_ptr<Value2> ArrayIndexValue(Value2& value, size_t offset)
{
	ArrayValue array{ 1,2,3,4,5,6,7,8,9,0 };

	CRY_UNUSED(value);
	CRY_UNUSED(offset);

	//ArrayAccess::woei(array, offset);

	return nullptr; //RecordProxy::MemberValue(value, name);
}

} // namespace Valuedef
} // namespace SubValue
} // namespace CryCC
