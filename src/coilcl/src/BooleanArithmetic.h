// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be
// copied and/or distributed without the express of the author.

#pragma once

#include <bitset>

namespace CoilCl
{

namespace Detail
{

struct BooleanOperator
{
	using type = bool;

	type value;

	BooleanOperator() = default;
	BooleanOperator(bool _value) : value{ _value } {}
	BooleanOperator(int _value) : value{ static_cast<bool>(_value) } {}
	~BooleanOperator() { Reset(); }

	operator bool() { return value; }

	void operator=(bool b) { value = b; }
	void operator=(int b) { value = static_cast<bool>(b); }

	void operator&=(BooleanOperator&& other)
	{
		Reset(new BooleanOperator{ std::move(other) });
	}

	void operator|=(BooleanOperator&& other)
	{
		Reset(new BooleanOperator{ std::move(other) });
	}

	void operator-=(BooleanOperator&& other)
	{
		Reset(new BooleanOperator{ std::move(other) });
	}

private:
	void Reset(BooleanOperator *_rhs = nullptr)
	{
		if (rhs) { delete rhs; }
		rhs = _rhs;
	}

private:
	BooleanOperator * rhs = nullptr;
};

} // namespace Detail

class BooleanArithmetic
{
	Detail::BooleanOperator m_truth;

public:
	BooleanArithmetic()
	{
		m_truth = true;

		m_truth &= false;
		m_truth |= false;
		m_truth -= false;

		if (m_truth) {

		}
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

} // namespace CoilCl
