// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <vector>
#include <deque>
#include <memory>

namespace CryExe
{

class Convert
{
public:
	using operations_type = int;

public:
	enum class Operations
	{
		CO_NONE = 1 << 0,
		CO_ENCRYPT = 1 << 1,
		CO_COMPRESS = 1 << 2,
	};

	friend void operator|=(operations_type& op1, Operations op2)
	{
		op1 |= static_cast<operations_type>(op2);
	}

	friend bool operator&(operations_type op1, Operations op2)
	{
		return op1 & static_cast<int>(op2);
	}

private:
	class AbstractConv
	{
		int m_prio;

	public:
		AbstractConv(int prio)
			: m_prio{ prio }
		{
		}

		virtual void Call(std::vector<uint8_t>&) = 0;

		friend bool operator>(const AbstractConv& conv1, const AbstractConv& conv2) { return conv1.m_prio > conv2.m_prio; }
		friend bool operator<(const AbstractConv& conv1, const AbstractConv& conv2) { return conv1.m_prio < conv2.m_prio; }
		friend bool operator>=(const AbstractConv& conv1, const AbstractConv& conv2) { return conv1.m_prio >= conv2.m_prio; }
		friend bool operator<=(const AbstractConv& conv1, const AbstractConv& conv2) { return conv1.m_prio <= conv2.m_prio; }
	};

	template<typename _Predicate>
	class CallableConv : public AbstractConv
	{
		_Predicate& m_pred;

	public:
		CallableConv(int prio, _Predicate& pred)
			: AbstractConv{ prio }
			, m_pred{ pred }
		{
		}

		void Call(std::vector<uint8_t>& data)
		{
			std::invoke(m_pred, data);
		}
	};

	static int GetPrioByConverterType(Operations operation);

private:
	template<template<typename> typename _Predicate, typename _EvalTy = std::shared_ptr<AbstractConv>>
	_EvalTy NextConverter()
	{
		std::sort(m_converters.begin(), m_converters.end(), _Predicate<_EvalTy>());
		return m_converters.front();
	}

public:
	Convert(std::vector<uint8_t>&, operations_type);

	void ToImage();
	void FromImage();

private:
	std::vector<uint8_t>& m_data;
	std::deque<std::shared_ptr<AbstractConv>> m_converters;
};

} // namespace CryExe
