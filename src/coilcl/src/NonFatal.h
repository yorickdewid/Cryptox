// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <deque>

#include <boost/format.hpp>

#define MAX_NOTICES	100

namespace CoilCl
{

//FUTURE:
// - Error codes
// - Source unit
// - Order by Type
class NonFatalNotice
{
	std::string m_message;
	struct Location {
		int line = -1;
		int col = -1;
	} m_location;

public:
	enum Type {
		E_WARNING,
		E_MESSAGE,
		E_HINT,
	};

private:
	Type m_type;

	static std::string TypeName(Type type)
	{
		switch (type)
		{
		case E_WARNING:
			return "warning";
		case E_HINT:
			return "hint";
		case E_MESSAGE:
		default:
			break;
		}

		return "";
	}

public:
	NonFatalNotice(std::string msg, Type type = Type::E_MESSAGE)
		: m_message{ msg }
		, m_type{ type }
	{
	}

	NonFatalNotice(std::string msg, std::pair<int, int> location, Type type = Type::E_MESSAGE)
		: m_message{ msg }
		, m_location{ location.first, location.second }
		, m_type{ type }
	{
	}

	Type Type() const noexcept { return m_type; }
	const std::string& Description() const noexcept { return m_message; }
	std::pair<int, int> Location() const noexcept { return { m_location.line, m_location.col }; }

	friend std::ostream& operator<<(std::ostream& os, const NonFatalNotice& notice)
	{
		auto typeName = TypeName(notice.m_type);
		if (!typeName.empty()) {
			typeName += ": ";
		}

		std::string location;
		if (!(notice.m_location.line == -1 && notice.m_location.col == -1)) {
			location = boost::format(boost::format{ " at <%1%,%2%>" } % notice.m_location.line % notice.m_location.col).str();
		}

		return os << typeName
			<< notice.m_message
			<< location;
	}
};

template<size_t _Count, typename _Ty = NonFatalNotice>
class NoticeDeque : private std::deque<_Ty>
{
	using _Myty = std::deque<_Ty>;

	// Push items on the deque until the deque is full. If the
	// deque is full pop the first items of the deque. The container
	// can never grow beyond the given size.
	void Enqueue(_Ty&& item)
	{
		if (IsFull())
		{
			pop_front();
		}
		push_back(std::move(item));
	}

public:
	using iterator = _Myty::iterator;
	using const_iterator = _Myty::const_iterator;

public:
	void Push(_Ty& item)
	{
		Enqueue(std::move(item));
	}

	void Push(_Ty&& item)
	{
		Enqueue(std::move(item));
	}

	template<typename... _ArgsTy>
	void Emplace(_ArgsTy... Args)
	{
		Enqueue(std::move(_Ty{ std::forward<_ArgsTy>(Args)... }));
	}

	iterator begin() noexcept { return _Myty::begin(); }
	const_iterator begin() const noexcept { return _Myty::begin(); }
	iterator end() noexcept { return _Myty::end(); }
	const_iterator end() const noexcept { return _Myty::end(); }

	inline void Empty() { Empty(); }
	inline bool IsFull() const noexcept { return size() == _Count; }
};

using DefaultNoticeList = NoticeDeque<MAX_NOTICES>;

extern DefaultNoticeList g_warningQueue;

namespace Util
{

void EnlistNotice(const std::string& msg, std::pair<int, int> location = {})
{
	g_warningQueue.Emplace(msg, location);
}

void EnlistNoticeWarning(const std::string& msg, std::pair<int, int> location = {})
{
	g_warningQueue.Emplace(msg, location, NonFatalNotice::E_WARNING);
}

void EnlistNoticeHint(const std::string& msg, std::pair<int, int> location = {})
{
	g_warningQueue.Emplace(msg, location, NonFatalNotice::E_HINT);
}

} // namespace Util
} // namespace CoilCl