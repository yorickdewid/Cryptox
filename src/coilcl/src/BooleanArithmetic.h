// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be
// copied and/or distributed without the express of the author.

#pragma once

#include <bitset>

#if 0

namespace CoilCl
{

namespace Detail
{

//enum LogicalOp
//{
//	And,
//	Or,
//};

//class BooleanAndOperator;
//class BooleanOrOperator;

struct OpAnd
{
};

struct OpOr
{
	bool operator()(bool self, bool other)
	{
		return false;
	}
};

struct BooleanOperator
{
};

template<typename _OperatorFunc>
struct BooleanOperatorImpl : public BooleanOperator
{
	using type = bool;

	type value;

	BooleanOperatorImpl(bool _value) : value{ _value } {}
	BooleanOperatorImpl(int _value) : value{ static_cast<type>(_value) } {}
	~BooleanOperatorImpl() { Reset(); }

	operator bool() { return Collapse(); }

	void operator=(bool b) { value = b; }
	void operator=(int b) { value = static_cast<type>(b); }

	BooleanOperatorImpl<OpAnd>& operator&=(bool b) noexcept
	{
		auto *ptr = new BooleanOperatorImpl<OpAnd>(b);
		Reset(ptr);
		return (*ptr);
	}

	BooleanOperatorImpl<OpOr>& operator|=(bool b) noexcept
	{
		auto *ptr = new BooleanOperatorImpl<OpOr>(b);
		Reset(ptr);
		return (*ptr);
	}

private:
	void Reset(BooleanOperator *_rhs = nullptr)
	{
		if (rhs) { delete rhs; }
		rhs = _rhs;
	}

	type Evaluate(type parent)
	{
		type result = consolidate(parent, value);
		if (rhs) {
			rhs->result(result);
		}

		return result;
	}

	type Collapse()
	{
		if (rhs) {
			return rhs->Evaluate(value));
		}

		return value;
	}

private:
	BooleanOperator * rhs = nullptr;
	_OperatorFunc consolidate;
};

//class BooleanOrOperator final : public BooleanOperator
//{
//	type Eval(type b)
//	{
//		return b || Collapse();
//	}
//
//public:
//	template<typename... _ArgsTy>
//	BooleanOrOperator(_ArgsTy... Args)
//		: BooleanOperator{ std::forward<_ArgsTy>(Args) }
//	{
//	}
//};
//
//class BooleanAndOperator final : public BooleanOperator
//{
//	type Eval(type b)
//	{
//		return b && Collapse();
//	}
//
//public:
//	template<typename... _ArgsTy>
//	BooleanAndOperator(_ArgsTy... Args)
//		: BooleanOperator{ std::forward<_ArgsTy>(Args) }
//	{
//	}
//};

} // namespace Detail
#if 0
class BooleanArithmetic
{
	Detail::BooleanOrOperator m_truth;

public:
	BooleanArithmetic()
	{
		/*Detail::BooleanOperator x{ false };
		x |= true;*/

		m_truth &= false;

		//m_truth = true;

		/*m_truth = true;

		m_truth &= false;
		m_truth |= false;

		if (m_truth) {

		}*/
	}

	// Resolve truth equation
	bool Resolve() {}

	// Logical operations
	void Conjunction() {}
	void Disjunction() {}
	void Negation() {}

	// Add boolean
	void operator+=(bool b)
	{
	}
};

#endif
} // namespace CoilCl

#endif
